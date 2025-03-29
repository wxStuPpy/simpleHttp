#pragma once

#include"Channel.h"

struct ChannelMap
{
    struct Channel **list;
    int size; // 记录数组的总大小
};

//初始化
struct ChannelMap*channelMapInit(int size);
//清空数据
void channelMapClear(struct ChannelMap*map);
//重新分配内存空间
bool makeMapRoom(struct ChannelMap*map,int newSize,int unitSize);//单元字节
