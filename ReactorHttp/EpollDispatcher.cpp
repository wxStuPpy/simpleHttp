#include "Dispatcher.h"
#include <sys/epoll.h>
#include <cstdio>
#include <cstdlib>
#include "unistd.h"

#define Max 520

struct EpollData
{
    int epfd;
    struct epoll_event *events;
};

static void *epollInit();

static int epollAdd(struct Channel *channel, struct EventLoop *evLoop);

static int epollRemove(struct Channel *channel, struct EventLoop *evLoop);

static int epollModify(struct Channel *channel, struct EventLoop *evLoop);

static int epollDispatch(struct EventLoop *evLoop, int timeout); // 单位s

static int epollClear(struct EventLoop *evLoop);

static int epollCtl(struct Channel *channel, struct EventLoop *evLoop, int op);

struct Dispatcher EpollDispatch = {
    epollInit,
    epollAdd,
    epollRemove,
    epollModify,
    epollDispatch,
    epollClear};

static int epollCtl(struct Channel *channel, struct EventLoop *evLoop, int op)
{
    struct EpollData *data = (struct EpollData *)(evLoop->dispatcherData);
    struct epoll_event ev;
    // 记录要检测的文件描述符
    ev.data.fd = channel->fd;
    // 记录检测的事件
    int events = 0;
    if (channel->events & ReadEvent)
    {
        events |= EPOLLIN;
    }
    if (channel->events & WriteEvent)
    {
        events |= EPOLLOUT;
    }
    ev.events = events;

    int ret = epoll_ctl(data->epfd, op, channel->fd, &ev);
    return ret;
}

static void *epollInit()
{
    struct EpollData *data = new EpollData;
    data->epfd = epoll_create(1);
    if (data->epfd == -1)
    {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }
    data->events = (struct epoll_event *)calloc(Max, sizeof(struct epoll_event));

    return data;
}

static int epollAdd(struct Channel *channel, struct EventLoop *evLoop)
{
    int ret = epollCtl(channel, evLoop, EPOLL_CTL_ADD);
    if (ret == -1)
    {
        perror("epoll_ctl_add");
    }
    return ret;
}

static int epollRemove(struct Channel *channel, struct EventLoop *evLoop)
{
    int ret = epollCtl(channel, evLoop, EPOLL_CTL_DEL);
    if (ret == -1)
    {
        perror("epoll_ctl_del");
    }
    return ret;
}

static int epollModify(struct Channel *channel, struct EventLoop *evLoop)
{
    int ret = epollCtl(channel, evLoop, EPOLL_CTL_DEL);
    if (ret == -1)
    {
        perror("epoll_ctl_mod");
    }
    return ret;
}

static int epollDispatch(struct EventLoop *evLoop, int timeout)
{
    struct EpollData *data = (struct EpollData *)(evLoop->dispatcherData);
    int count=epoll_wait(data->epfd,data->events,Max,timeout*1000);
    if(count==-1){
        perror("epoll_wait");
        return -1;
    }
    for(int i=0;i<count;++i){
    int events=data->events[i].events;
    int fd=data->events[i].data.fd;
    if(events&EPOLLERR||events&EPOLLHUP){
        //epollRemove(Channel,evLoop);
        continue;
    }
    if(events&EPOLLIN){

    }
    if(events&EPOLLOUT){

    }
    }
}

static int epollClear(struct EventLoop *evLoop)
{
    struct EpollData *data = (struct EpollData *)(evLoop->dispatcherData);
    free(data->events);
    close(data->epfd);
    free(data);
}