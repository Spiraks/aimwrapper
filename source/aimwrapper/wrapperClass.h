#ifndef WRAPPERCLASS_H
#define WRAPPERCLASS_H

#include "item.h"

#include <iostream>
#include <queue>

class WrapperClass
{
public:
    WrapperClass();
    void startListening();
    void closeSocket();
    void clearQueue();
    void clearAll();
    
    Item getItemQueue();
    
    Item Cleared = Item();
    int PAGE_SIZE;
    std::string NamePortGPS;

private:
    bool workFlag = true;
    std::queue<Item> queue_items;

};
void udpListnerThread(std::queue<Item> &queue, bool &socketWorkFlag);
void diep(char *s);
#endif