#pragma once

#include"Channel.h"
#include"EventLoop.h"

struct Dispatcher
{
    //init 初始化epoll poll或者select需要的数据块(泛型编程)
    void*(*init)();
    //add  第一个参数记录文件描述符 第二个参数记录poll epoll or select 以及对应的参数
    int(*add)(struct Channel*channel,struct EventLoop *evLoop);
    //remove
    int(*remove)(struct Channel*channel,struct EventLoop *evLoop);
    //modify
    int(*modify)(struct Channel*channel,struct EventLoop *evLoop);
    //dispatch
    int(*dispatch)(struct EventLoop *evLoop,int timeout);//单位s
    //clear
    int(*clear)(struct EventLoop *evLoop);
};
