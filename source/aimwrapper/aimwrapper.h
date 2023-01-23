#ifndef AIMWRAPPER_H
#define AIMWRAPPER_H

#include <condition_variable>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <queue>

#include <cstring>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <stdexcept>
#include <list>
#include <vector>
#include <ctime>

#include <math.h>
#include <map>

#include "item.h"
#include "wrapperClass.h"



#define FADC 1000000000

//#define PAGE_SIZE 15000


using namespace std;


enum FlagPolarisation{
    VV = 0,
    HH = 1,
    HV = 2,
    VH = 3,
    VV_HH = 4,
    HH_HV = 5,
    VV_VH = 6,
    HH_HV_VH_VV = 7,
    AUTO = 8
};
class ItemString
{
public:
    ItemString();
    void startListeningString(const string & a);
    char *getFlagPage();

    void stopListeningString();
    void clearQueue();
    void clearAll();
    //int PAGE_SIZE=15000;

private:
    WrapperClass WC;
    std::string NameOutFile;
    queue<char*> queue_page_counter;
    bool workFlag = true;
};
struct int2b{
    unsigned a:2;
};


enum FlagData{
    OK = 2,
    BAD_STR = 1,
    UNNOWN = 0
};




#endif // AIMWRAPPER_H
