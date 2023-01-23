
#include "item.h"
#include "wrapperClass.h"

#include <iostream>
#include <thread>
#include <fstream>
#include <unistd.h> // write(), read(), close()
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <queue>

using namespace std;

// Класс, реализующий чтение в отдельном потоке датаграмм из UDP сокета и формирование целостных строк данных
// (либо отлавливание битой строки) для передачи в ItemPage, который в свою очередь формирует и записывает страницу
// данных на носитель
WrapperClass::WrapperClass()
{}
void WrapperClass::startListening()
{
    // Запуск процесса считывания датаграмм из сокета
    workFlag = true;
    std::thread listenerT(  udpListnerThread,
                            std::ref(queue_items),
                            std::ref(workFlag));
    listenerT.detach();
}

void WrapperClass::closeSocket()
{
    workFlag = false;
}

void WrapperClass::clearQueue()
{
    while ( !queue_items.empty() )
    {
        Item element = queue_items.front();
        //free(element.data);
        queue_items.pop();
    }
}

void WrapperClass::clearAll()
{
    closeSocket();
    clearQueue();
}

Item WrapperClass::getItemQueue()
{
    if ( queue_items.empty() )
    {
        return Cleared;
    }
    Item element = queue_items.front();
    queue_items.pop();

    return element;
}

void udpListnerThread(  std::queue<Item> &queue,    //
                        bool &socketWorkFlag)       //
{
    cout << "Start listening" << endl;
    ofstream fout;
    fout.open("logs/parser.txt");
    fout << "Start listening" << endl;
    //===============================
    uint16_t PORT = 8888;
    const char* IP =  "10.0.0.1";
    cout << "IP = " << IP << endl;
    cout << "PORT = " << PORT << endl;
    //===============================
    size_t BUFLEN = 100000;
    //    bool ConstTest = true;


    // Количество байт которое должно считаться из сокета,
    // по факту успеваем выхватывать каждую только что принятую датаграмму длиной 1472 байт (У меня считается 1464)
    int recv_bytes_number = 2000;


    char hchars[] = {0x33, 0x22, 0x33, 0x33}; // Header строки данных от РЛС "Элиарс"

    char Buffer[BUFLEN];  // Буффер приема пакетов

    // Структура для считывания из сокета
    struct sockaddr_in si_me, si_other;
    int sizeSock = sizeof(si_other);
    unsigned int slen = sizeof(si_other);

    uint bufinque = 0;  // Текущее количество данных в буффере приема пакетов

    uint retu_recv=0;  // Количество считанных байт из сокета

    uint packet_number = 0; // Порядковый	номер полученного сообщения

    uint line_leng = 30000;  // Стартовое значение длины строки данных (заведомо больше любого принятого)
    uint line_leng_start = 50000;

    // Переменные для считывания размера системного приемного буфера сокета
    int rcvbuf = 0;
    uint len_sock_buf = sizeof(rcvbuf);
    //std::ofstream out1;   @ For huge one file saving!!!
    //out1.open("ReadInWrap.bin",ios::binary);   @ For huge one file saving!!!


    cout << "Try to create socket ..." << endl;

    if ((sizeSock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)  //  Проблема с сокетом
    {
        cout << "failed to creat socket" << endl;
        diep((char *)"socket");
    }
    else {  // Все хорошо сокет открылся
        cout << "successful socket creation" << endl;
        getsockopt(sizeSock,  SOL_SOCKET, SO_RCVBUF, &rcvbuf, &len_sock_buf);
        cout << "Default receiving buffer size: " << rcvbuf << endl;
        // Установка флага для перебинда к сокету при падении Питона
        int enable = 1;
        if (setsockopt(sizeSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            cout << "setsockopt(SO_REUSEADDR) failed" << endl;


    }

    memset((char *) &si_me, 0, sizeof(si_me));
    memset((char *) &si_other, 0, sizeof(si_other));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = inet_addr(IP);

    if (bind(sizeSock, (struct sockaddr *)&si_me, sizeof(si_me)) == -1) //  Не подключился
    {
        cout << "no bind" << endl;
        diep((char *)"bind");
    }
    else {  //  Подключился всё хорошо
        cout << "Successfuly bind!" << endl << endl;
    }

    cout << "Start receiving parsels..."<< endl;
    //бесконечный цикл для взятия данных из сокета
    while(1)  {
        if (socketWorkFlag == false) //если ли проблема в сокете
        {
            cout << "Closing socket ..." << endl;
            if ( close(sizeSock) == - 1 ) {
                cout << "Can't close socket!" << endl;
                diep((char*)"socket closing");
            }
            else
            {
                cout << "Socket was successfuly  closed"<<endl;;
            }
            break;
        }


        // Считывание данных из сокета и постепенное заполнение буффера
        // В тестах получается так, что за раз успеваем считать одну вновь принятую датаграмму 1472 байта
        retu_recv = recvfrom(sizeSock, Buffer+bufinque, recv_bytes_number, 0,  (struct sockaddr *)&si_other, &slen);
        bufinque+=retu_recv;  // Обновление количества байт данных в буфере исходя из количества принятых байт из сокета
        //        out<<"Current buffer data size: "<<bufinque<<endl;  // Выводит длину буфера в логер
        //cout<<"Recv from result: "<<retu_recv<<endl;

        // Начало обработки при накоплении в буфере данных больше одного ожидаемого сообщения
        if(bufinque > line_leng_start)//если буфер больше длины строки ищем заголовки
        {

            uint ix = 1;  // Индекс найденого заголовка сообщения в буфере, начинаем с 1 чтобы отшагнуть от заголовка в начале буффера

            // Цикл поиска заголовка в буфере
            // Сделана проверка что заголовок лежит не вконце буффера и что можно будет считать параметр line_leng. Было: (ix!=bufinque)
            while (!(Buffer[ix] == hchars[0] && Buffer[ix+1] == hchars[1] && Buffer[ix+2] == hchars[2] && Buffer[ix+3] == hchars[3])&&(ix < bufinque-8)) {
                ix++;
            }
            //cout<<"ix"<<ix <<endl;

            memcpy(&line_leng, Buffer+4, 4);
            
            int real_line_leng_mul = line_leng/1464+(line_leng%1464 ? 1 :0);
            //cout<<line_leng<<' '<<real_line_leng_mul<<endl;
            //cout<<"Header index: "<<ix<<"    Estimated line length: "<<line_leng/1464+(line_leng%1464 ? 1 :0)<<"    Current buffer data size: "<<bufinque<<endl;

            if(ix==real_line_leng_mul*1464) {
                //cout<<"Good data: "<<endl;

                Item tmp;
                tmp.dataFlag = 2;
                char data1[line_leng];
                //                 = data1;
                memcpy(data1, Buffer, line_leng);
                tmp.data = std::string(data1, line_leng);

                tmp.Leng = line_leng;

                memcpy(&packet_number, Buffer+12, 4);
                //out<<"Parsel Number: "<<packet_number<<endl;

                //cout<<"STD::size: "<<q.size()<<endl;
                //out<<"Header: "<<Buffer[ix]<<Buffer[ix+1]<<Buffer[ix+2]<<Buffer[ix+3]<<endl;
                // cout<<tmp.data;//  @ For huge one file saving!!!
                queue.push(tmp);

                // Перестановка начала нового сообщения в начало буффера
                uint n = 0;
                while(n<bufinque-ix)
                {
                    Buffer[n] = Buffer[ix+n];
                    n++;
                }
                bufinque-=ix; // Обновление количества байт данных в буффере
            }

            else {
                Item tmp;
                //out<<"Bad data: "<<1<<endl;
                tmp.dataFlag = 1;
                tmp.Leng = line_leng;

                queue.push(tmp);

                memcpy(&packet_number, Buffer+12, 4);
                //out<<"Parsel Number: "<<packet_number<<endl;

                // Перестановка начала нового сообщения в начало буффера
                uint n = 0;
                while(n<bufinque-ix)
                {
                    Buffer[n] = Buffer[ix+n];
                    n++;
                }
                bufinque-=ix;  // Обновление количества байт данных в буффере

            }
        }

    }
}

void diep(char *s)
{
    perror(s);
    exit(1);
}