#include <iostream>
#include "server.h"
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "./a.out port path\n";
        return -1;
    }
    unsigned short port=atoi(argv[1]);
    //切换服务器的工作路径
    chdir(argv[2]);
    // 初始化用于监听的套接字
    int lfd = initListenFD(port);
    // 启动服务器程序
    epollRun(lfd);
    return 0;
}