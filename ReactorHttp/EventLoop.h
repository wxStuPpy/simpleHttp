#pragma once

#include"Dispatcher.h"

extern struct Dispatcher EpollDispatch;
struct EventLoop
{
    Dispatcher*dispatcher;
    void*dispatcherData;
};
