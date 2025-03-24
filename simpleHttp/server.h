#pragma once

//初始化监听套接字
int initListenFD(unsigned short port);

//启动epoll
int epollRun(int lfd);

//与客户端建立连接
int acceptClient(int lfd,int epfd);

//接收http请求
int recvHttpRequest(int cfd,int epfd);

//解析请求行
int parseRequstLine(const char*line,int cfd);

//发送文件
int sendFile(const char *fileName,int cfd);

//发送响应头(状态行+响应头)
int sendHeadMsg(int cfd,int status,const char*descr,const char*type,int len);
const char*getFileType(const char*name);

//发送目录
int sendDir(const char*dirName,int cfd);

