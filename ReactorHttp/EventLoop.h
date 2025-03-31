#pragma once

#include"Dispatcher.h"

extern struct Dispatcher EpollDispatch;
struct EventLoop
{
    Dispatcher*dispatcher;//六个函数指针
    void*dispatcherData;
};
