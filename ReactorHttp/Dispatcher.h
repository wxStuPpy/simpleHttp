#pragma once

#include"Channel.h"
#include"EventLoop.h"

struct Dispatcher
{
    //init 初始化epoll poll或者select需要的数据块(泛型编程)
    void*(*init)();
    //add
    void(*add)(struct Channel*channel,struct EventLoop *evLoop);
    //remove
    void(*remove)(struct Channel*channel,struct EventLoop *evLoop);
    //modify
    void(*modify)(struct Channel*channel,struct EventLoop *evLoop);
    //dispatch
    int(*dispatch)(struct EventLoop *evLoop,int timeout);//单位s
    //clear
    int(*clear)(struct EventLoop *evLoop);
};
