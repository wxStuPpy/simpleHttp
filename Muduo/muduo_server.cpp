#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <iostream>
#include <functional>

using namespace muduo::net;
using namespace muduo;
using namespace std::placeholders;

/// @brief
class ChatServer
{
public:
    /// @brief
    /// @param loop 事件循环 Reactor
    /// @param listenAddr ip地址+端口
    /// @param nameArg 服务器的名称
    ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg)
        : m_server(loop, listenAddr, nameArg), m_loop(loop)
    {
        // 给服务器注册用户连接的创建和断开回调
        m_server.setConnectionCallback(std::bind(&onConnenction, this, _1));
        // 给服务器针对注册用户读写事件回调
        m_server.setMessageCallback(std::bind(&onMessage, this, _1, _2, _3));
        // 设置服务器端的线程数量 1个io线程 4个工作线程
        m_server.setThreadNum(5);
    }
    // 开启事件循环
    void start()
    {
        m_server.start();
    }

private:
    // 专门处理用户的连接和断开
    void onConnenction(const TcpConnectionPtr &conn)
    {
        if(conn->connected()){

        }
        else{
            
        }
    }
    // 专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time)
    {
    }

    TcpServer m_server;
    EventLoop *m_loop; // epoll
};

int main(){
    EventLoop loop;//epoll
    InetAddress addr("127.0.0.1",6000);
    ChatServer server(&loop,addr,"ChatServer");
    server.start();// listenfd epoll_ctl
    loop.loop();//epoll_wait

}