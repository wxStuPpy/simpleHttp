#include "ChannelMap.h"
#include <cstdlib>
#include <cstring>

ChannelMap *channelMapInit(int size)
{
    struct ChannelMap *map = new ChannelMap;
    map->size = size;
    map->list = new Channel *[size];
    return map;
}

void channelMapClear(ChannelMap *map)
{
    if (map)
    {
        for (int i = 0; i < map->size; ++i)
        {
            if (map->list[i])
                delete map->list[i];
        }
        delete[] map->list;
        map->list = nullptr;
    }
    map->size = 0;
}

bool makeMapRoom(ChannelMap *map, int newSize, int unitSize)
{
    int curSize = map->size;
    while (curSize < map->size)
    {
        curSize = curSize * 2; // 容量每次扩大一倍
    }
    // 扩容realloc
    struct Channel **temp = (Channel **)realloc(map->list, curSize * unitSize);
    if (!temp)
    {
        return false;
    }
    map->list = temp;
    memset(&map->list[map->size], 0, (curSize - map->size) * unitSize);
    map->size = curSize;
    return true;
}
