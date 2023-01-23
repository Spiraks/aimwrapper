#ifndef ITEM_H
#define ITEM_H

#include <string>

class Item  // Контейнер для радиолокационных данных.
{
public:
    Item();
    int dataFlag = 0; // 0 - no new data string, 1 - corrupted data string, 2 - valid data string
    std::string data = "";
    int Leng=0;

    int getDataFlag();  // Возвращает метку текущих данных
    int getLengs();  // Возвращает байтовую длину текущей строки
    std::string getData();  // Возвращает
    std::string getPyStrData();
    void clearData();

};

#endif