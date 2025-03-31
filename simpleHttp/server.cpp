#include "server.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/sendfile.h>
#include <dirent.h>
#include <cstdlib>
#include <ctype.h>
#include <pthread.h>

struct FDInfo{
    int fd;
    int epfd;
    pthread_t tid;
};

int initListenFD(unsigned short port)
{
    // 1.创建监听fd
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        return -1;
    }
    // 2.设置端口复用
    int opt = 1;
    int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret == -1)
    {
        perror("setsockopt");
        return -1;
    }
    // 3.绑定
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(lfd, (sockaddr *)&addr, sizeof(sockaddr));
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }
    // 4.设置监听
    ret = listen(lfd, 128);
    if (ret == -1)
    {
        perror("listen");
        return -1;
    }

    return lfd;
}

int epollRun(int lfd)
{ // 1.创建epoll实例 参数已经没有意义 大于0即可
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("epoll_create");
        return -1;
    }
    // 2.lfd上树
    struct epoll_event ev;
    ev.data.fd = lfd;
    // 判断有无客户端连接 对应读事件 epollin
    ev.events = EPOLLIN;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl");
        return -1;
    }
    // 3.持续检测有无连接
    // 创建传出参数结构体
    struct epoll_event evs[1024];

    while (1)
    {
        int num = epoll_wait(epfd, evs, sizeof(evs) / sizeof(evs[0]), -1); //-1代表阻塞
        for (int i = 0; i < num; ++i)
        {   struct FDInfo*info=new FDInfo;
            int fd = evs[i].data.fd;
            info->fd=fd;
            info->epfd=epfd;
            // 监听描述符
            if (fd == lfd)
            {
                // 建立新链接
                //acceptClient(lfd, epfd);
                pthread_create(&info->tid,nullptr,acceptClient,info);
                
            }
            // 通信描述符
            else
            {
                // 主要是接受对端的数据 (http协议)
                //recvHttpRequest(fd, epfd);
                pthread_create(&info->tid,nullptr,recvHttpRequest,info);
            }
        }
    }

    return 0;
}

void* acceptClient(void*arg)
{ // 1.建立连接
    FDInfo *info=(FDInfo*)arg;
    int lfd=info->fd;
    int epfd=info->epfd;
    printf("222");
    int cfd = accept(lfd, nullptr, nullptr);
    if (cfd == -1)
    {
        perror("accept");
        return nullptr;
    }
    // 2.设置非阻塞 NNN
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    // 3.将文件描述符cfd添加到epoll树
    struct epoll_event ev;
    ev.data.fd = cfd;
    // 边沿非阻塞
    ev.events = EPOLLIN | EPOLLET;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl");
        return nullptr;
    }
    delete info;
    return nullptr;
}

void* recvHttpRequest(void*arg)
{   FDInfo *info=(FDInfo*)arg;
    int cfd=info->fd;
    int epfd=info->epfd;
    int len = 0;
    size_t total = 0;
    char tmp[1024] = {0};
    char buf[4096] = {0};
    while ((len = recv(cfd, tmp, sizeof(tmp), 0)) > 0)
    {
        if (total + len < sizeof(buf))
        {
            memcpy(buf + total, tmp, len);
        }
        total += len;
        memset(tmp, 0, sizeof(tmp));
    }
    // 判断数据是否接收完毕
    if (len == -1 && errno == EAGAIN)
    {
        // 解析请求行
        char *pt = strstr(buf, "\r\n");
        int reqLen = pt - buf;
        buf[reqLen] = '\0';
        parseRequstLine(buf, cfd);
    }
    else if (len == 0)
    {
        // 客户端断开连接
        epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, nullptr);
        close(cfd);
    }
    else
    {
        perror("recv");
    }
    delete info;
    return nullptr;
}

int parseRequstLine(const char *line, int cfd)
{ // 解析请求行  GET /index.html HTTP/1.1
    char method[12];
    char path[1024]; 
    sscanf(line, "%[^ ] %[^ ]", method, path);
    // 不区分大小写比较
    if (strcasecmp(method, "get") != 0)
    {
        return -1;
    }
    //遍历自身 处理中文和特殊符号
    decodeMsg(path,path);
    // 处理客户端请求的静态资源
    char *file = nullptr;
    if (strcmp(path, "/") == 0)
    {
        file = "./";
    }
    else
    {
        file = path + 1;
    }

    struct stat st;
    int ret = stat(file, &st);
    if (ret == -1)
    {
        // 文件不存在 回复404
        sendHeadMsg(cfd, 404, "Not Found", getFileType(".html"), -1); //-1让浏览器自己去读长度
        sendFile("404.html", cfd);
        return 0;
    }
    // 判断请求的资源是目录还是文件
    if (S_ISDIR(st.st_mode))
    {
        // 把这个目录中的内容发给客户端
        sendHeadMsg(cfd, 200, "OK", getFileType(".html"), -1);
        sendDir(file, cfd);
    }
    else
    {
        // 把这个文件中的内容发给客户端
        sendHeadMsg(cfd, 200, "OK", getFileType(file), st.st_size);
        sendFile(file, cfd);
    }

    return 0;
}

const char *getFileType(const char *name)
{
    // 自右向左找'.' 不存在则返回默认类型
    const char *dot = strrchr(name, '.');
    if (dot == nullptr)
    {
        return "text/plain; charset=utf-8";
    }

    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
    {
        return "text/html; charset=utf-8";
    }
    else if (strcmp(dot, ".css") == 0)
    {
        return "text/css; charset=utf-8";
    }
    else if (strcmp(dot, ".js") == 0)
    {
        return "application/javascript; charset=utf-8";
    }
    else if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
    {
        return "image/jpeg";
    }
    else if (strcmp(dot, ".png") == 0)
    {
        return "image/png";
    }
    else if (strcmp(dot, ".gif") == 0)
    {
        return "image/gif";
    }
    else if (strcmp(dot, ".svg") == 0)
    {
        return "image/svg+xml";
    }
    else if (strcmp(dot, ".mp4") == 0)
    {
        return "video/mp4";
    }
    else if (strcmp(dot, ".pdf") == 0)
    {
        return "application/pdf";
    }
    else if (strcmp(dot, ".json") == 0)
    {
        return "application/json; charset=utf-8";
    }

    return "text/plain; charset=utf-8";
}

int sendDir(const char *dirName, int cfd)
{
    char buf[4096] = {0};
    sprintf(buf, "<html><head><title>%s</title><head><body><table>", dirName);
    struct dirent **namelist;
    int dirNum = scandir(dirName, &namelist, nullptr, alphasort);
    for (int i = 0; i < dirNum; ++i)
    {
        // 取出文件名
        char *name = namelist[i]->d_name;
        struct stat st;
        char subPath[1024] = {0};
        sprintf(subPath, "%s/%s", dirName, name);
        stat(subPath, &st);
        // 判断子项目还是不是目录
        if (S_ISDIR(st.st_mode))
        { // a标签 <a href="">name</a>
            sprintf(buf + strlen(buf), "<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td><tr>",
                    name, name, st.st_size);
        }
        else
        {
            sprintf(buf + strlen(buf), "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td><tr>", // 第一个%s不需要"/"
                    name, name, st.st_size);
        }
        send(cfd, buf, strlen(buf), 0);
        memset(buf, 0, sizeof(buf));
        free(namelist[i]);
    }
    sprintf(buf, "</table></body></html>");
    send(cfd, buf, strlen(buf), 0);
    free(namelist);
    return 0;
}

int sendFile(const char *fileName, int cfd)
{ // 1.打开文件
    int fd = open(fileName, O_RDONLY);
    assert(fd > 0);
#if 0 // 该方法比较麻烦
    while(1){
        char buf[1024];
        int len=read(fd,buf,sizeof(buf));
        if(len>0){
            send(cfd,buf,len,0);
            //休眠10微秒 防止发送太频繁 很重要 
            usleep(10);
        }
        else if(len==0){
            break;
        }
        else{
            perror("read");
        }
        }
        close(fd);
#else
    // 求文件的大小 或者使用stat
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    off_t offset = 0;

    while (offset < size)
    {
        sendfile(cfd, fd, &offset, size-offset);
    }

    close(fd);
#endif
    return 0;
}

int sendHeadMsg(int cfd, int status, const char *descr, const char *type, int len)
{ // 状态行
    char buf[4096] = {0};
    sprintf(buf, "http/1.1 %d %s\r\n", status, descr);
    // 响应头
    sprintf(buf + strlen(buf), "content-type:%s\r\n", type);
    sprintf(buf + strlen(buf), "content-length:%d\r\n\r\n", len); // 包括空行

    send(cfd, buf, strlen(buf), 0);
    return 0;
}

// 将十六进制转化为十进制
int hexToDec(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

//解码
void decodeMsg(char *to, char *from)
{
    for (; *from - '\0'; ++to, ++from)
    {
        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
        {
            *to = hexToDec(from[1] * 16 + hexToDec(from[2]));
            from += 2;
        }
        else
        {
            *to = *from;
        }
    }
}