#include "item.h"
#include <iostream>

using namespace std;
Item::Item(){}

std::string Item::getData()
{
    return data;
}
void Item::clearData()
{
    cout << "Item::clearData()    before"<<endl;
    //data.clea
    cout << "Item::clearData()    after"<<endl;
}
std::string Item::getPyStrData()
{
    std::string s = data;
    return s;
}

int Item::getDataFlag()
{
    return dataFlag;
}

int Item::getLengs()
{
    return Leng;
}