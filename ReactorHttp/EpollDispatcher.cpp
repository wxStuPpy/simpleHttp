#include "Dispatcher.h"
#include <sys/epoll.h>
#include <cstdio>
#include <cstdlib>

#define Max 520

struct EpollData
{
    int epfd;
    struct epoll_event *events;
};

static void *epollInit();

static void epollAdd(struct Channel *channel, struct EventLoop *evLoop);

static void epollRemove(struct Channel *channel, struct EventLoop *evLoop);

static void epollModify(struct Channel *channel, struct EventLoop *evLoop);

static int epollDispatch(struct EventLoop *evLoop, int timeout); // 单位s

static int epollClear(struct EventLoop *evLoop);

struct Dispatcher EpollDispatch = {
    epollInit,
    epollAdd,
    epollRemove,
    epollModify,
    epollDispatch,
    epollClear};

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

static void epollAdd(struct Channel *channel, struct EventLoop *evLoop)
{
}

static void epollRemove(struct Channel *channel, struct EventLoop *evLoop)
{
}

static void epollModify(struct Channel *channel, struct EventLoop *evLoop)
{
}

static int epollDispatch(struct EventLoop *evLoop, int timeout)
{
}

static int epollClear(struct EventLoop *evLoop)
{
}