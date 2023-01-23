#include "aimwrapper.h"
#include "fstream"
#include <stdexcept>
#include <list>
#include <vector>
#include <algorithm>
//#include <boost/lockfree/queue.hpp>
//#include <boost/atomic.hpp>
#include <ctime>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <queue>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include <dirent.h>
#include <cstdlib>
#include <stdio.h>






using namespace std;

std::map<int,vector<string>> polarisation = {   {0, vector<string>{"VV"}},
                                                {1, vector<string>{"HH"}},
                                                {2, vector<string>{"HV"}},
                                                {3, vector<string>{"VH"}},
                                                {4, vector<string>{"VV","HH"}},
                                                {5, vector<string>{"HH","HV"}},
                                                {6, vector<string>{"VV","VH"}},
                                                {7, vector<string>{"HH","HV","VH","VV"}},
                                                };
std::map<int,string> local_polarisation = {     {0, "HH"},
                                                {1, "HV"},
                                                {2, "VH"},
                                                {3, "VV"}};

ItemString::ItemString()
{
    NameOutFile = "";//не в коем случае не удалять !!! остановись!!!
}

int countOfFile(const char* pathDirectory)
{
    DIR *dir;
    if ((dir=opendir(pathDirectory))==nullptr)
    {
        printf("Can\'t open folder\n");
        return -1;
    }
    else
    {
        int namberOfFile = 0;
        struct dirent *f_cur;
        while (readdir(dir)!=nullptr)
        {
			f_cur=readdir(dir);
            namberOfFile++;
        }
        return namberOfFile;
    }
}

int countOfFile1(const char* pathDirectory)
{
	struct dirent **list;
    int n = scandir(pathDirectory,&list,NULL,NULL);
    int Filecount = 0 ;
    for(int i = 0; i<n;i++)
    {
		if(list[i]->d_type!=4)
		{
			Filecount++;
		}
	}
	//cout<< n << ' ' <<Filecount<<endl;
	return  Filecount;
    
}

int DelFileInDir(const char* pathDirectory)
{
    DIR *dir;
    if ((dir=opendir(pathDirectory))==nullptr)
    {
        printf("Can\'t open folder\n");
        return -1;
    }
    else
    {
        //        int naberOfFile = 0;
        struct dirent *f_cur;
        while ((f_cur=readdir(dir))!=nullptr)
        {
            string a=string(pathDirectory)+string(f_cur->d_name);
            remove(a.c_str());
        }
        return 1;
    }
}


string getFileName(int n,int page)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    time_t ttime = time(0);
    tm *local_time = localtime(&ttime);
    std::string str;
    if ( ltm->tm_mday < 10 )
    {
        str.append("0");
    }
    str.append(std::to_string(ltm->tm_mday));
    str.append(".");
    if ( 1 + ltm->tm_mon < 10 )
    {
        str.append("0");
    }
    str.append(std::to_string(1 + ltm->tm_mon));
    str.append(".");
    str.append(std::to_string(1900 + ltm->tm_year));
    str.append("_");
    if ( local_time->tm_hour < 10 )
    {
        str.append("0");
    }
    str.append(std::to_string(local_time->tm_hour));
    str.append(".");
    if ( local_time->tm_min < 10 )
    {
        str.append("0");
    }
    str.append(std::to_string(local_time->tm_min));
    str.append(".");
    if ( local_time->tm_sec < 10 )
    {
        str.append("0");
    }

    str.append(std::to_string(local_time->tm_sec));
    str.append("_");
    if ( n < 10 )
    {
        str.append("0000");
    }
    else if ( n < 100 )
    {
        str.append("000");
    }
    else if ( n < 1000 )
    {
        str.append("00");
    }
    else if ( n < 10000 )
    {
        str.append("0");
    }
    str.append(std::to_string(n));
    str.append("_Page_");
    if ( page < 10 )
    {
        str.append("0000");
    }
    else if ( page < 100 )
    {
        str.append("000");
    }
    else if ( page < 1000 )
    {
        str.append("00");
    }
    else if ( page < 10000 )
    {
        str.append("0");
    }
    str.append(std::to_string(page)); 
    
    return str;
}

void WorkWithDataOK(    Item &item, 
                        std::map<std::string,std::ofstream> &out, 
                        FlagPolarisation &flagPolarization, 
                        int &numberPage,
                        const char* &export_path,
                        vector<string> &polarisation_now)
{
    string dataString = item.getData();
    int length = item.getLengs();
    int numberPolarisation;
    if(polarisation_now.empty()){
        cout<<"Polarisation is empty" << flagPolarization <<endl;
        numberPolarisation =int(dataString[102])%16;
        polarisation_now = polarisation[numberPolarisation];
        if(numberPolarisation == flagPolarization || flagPolarization == AUTO) //устанавливаем режим работы программы AUTO - любая поляризация
        { 
            string pageName = getFileName(840,numberPage);

            for (int m=0;m<polarisation_now.size();m++)
            {
                out[polarisation_now[m]]=ofstream(export_path + pageName + "_" +polarisation_now[m] + ".bin",ios::binary);
                if(!out[polarisation_now[m]].is_open())
                {
                    cout<<"Ошибка открытия файла "<< polarisation_now[m] <<endl;
                    continue; 
                }
                else
                {
                    cout<<"Файл открыт "<< polarisation_now[m]<<endl;
                }
            }
        }
        else
        {
            cout<<"Поляризация не найдена\n";
            return;    
        }
    }
    int2b nomber_other_pol;
    nomber_other_pol.a = int(dataString[103])/16;
    string poll = local_polarisation[nomber_other_pol.a];
    vector<string>::iterator it = find(polarisation_now.begin(), polarisation_now.end(), poll);

    if (it == polarisation_now.end())
    {
        cout<< "Polarization was not found!" <<endl;
    }
    else
    {
        cout<< "Polarization found at index: " << it - polarisation_now.begin() << "   lenght - "<< length <<endl;
        polarisation_now.erase(it);
        vector<string>(polarisation_now).swap(polarisation_now);
        out[poll]<<dataString;
        out[poll].close();
    }
}

void ItemRead(  WrapperClass &wc,
                std::string &S,
                queue<char*> &q,
                bool &readProcessWorkFlag, 
                int page_size)
{
    FlagPolarisation flagPolarization = AUTO;
    std::map<std::string,std::ofstream> out;
    const char* export_path = "/home/alex/Документы/WrapperUpdate/FileAim/";
    int numberPage = 0;
    int counterString = 0;
    int sizePolarisation = 0;
    vector<string> polarisation_now;
    
    wc.startListening();
    Item item;
    int dataFlag;

    
	clock_t oldTime = clock();
    while(readProcessWorkFlag) 
    {
        item = wc.getItemQueue();
        dataFlag = item.getDataFlag();
        
		while(((float(clock()-oldTime))/CLOCKS_PER_SEC)<2){		} // ждем пока не прошло 1 cek
		
        if(dataFlag == OK)
        {
            WorkWithDataOK(item, out, flagPolarization, numberPage, export_path, polarisation_now);
            if(counterString == 0)
            {
                sizePolarisation = polarisation_now.size();
            }
            if(!polarisation_now.empty())
            {
                counterString++;
                if(counterString == sizePolarisation)
                {
                    
                }
            }
            else
            {
                counterString = 0;
                numberPage++;
                oldTime = clock();
            }
        }
        else{
           counterString++;
        }
               
    }
    if(countOfFile1(export_path)>100)//считать файлы с полярезацией 
    {
        DelFileInDir(export_path);
    }
    return;
}

void ItemString::startListeningString(const string &a)
{
    int pageSizeFromPy = std::stoi(a);
    WC.PAGE_SIZE = pageSizeFromPy;
    workFlag = true;
    std::thread listenerT(  ItemRead,
                            std::ref(WC), 
                            std::ref(NameOutFile),
                            std::ref(queue_page_counter), 
                            std::ref(workFlag),
                            std::ref(pageSizeFromPy));
    listenerT.detach();
}

void ItemString::stopListeningString()
{
    cout << "stopListeningString()" << endl;
    workFlag = false;
    WC.closeSocket();
}

void ItemString::clearQueue()
{
    WC.clearQueue();
}

void ItemString::clearAll()
{
    WC.clearAll();
}

char* ItemString::getFlagPage()
{
    if ( queue_page_counter.empty() )
    {
        return 0;
    }
    char* element = queue_page_counter.front();
    queue_page_counter.pop();

    return element;
}