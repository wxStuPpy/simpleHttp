#include "Channel.h"

Channel *channelInit(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void *arg)
{
    struct Channel*channel=new Channel;
    channel->fd=fd;
    channel->events=events;
    channel->readCallback=readFunc;
    channel->writeCallback=writeFunc;
    channel->arg=arg;
    return channel;
}

void writeEventEnable(Channel *channel, bool flag)
{
    if(flag==true){
        channel->events|=WriteEvent;
    }
    else{
        //清零
        channel->events=channel->events&~WriteEvent;
    }
}

bool isWriteEventEnable(Channel *channel)
{
    return channel->events&WriteEvent;
}
