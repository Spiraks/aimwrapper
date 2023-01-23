#if defined(_DEBUG)
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif
#include <pybind11/embed.h>
#include <iostream>

#include <stdio.h>

/* void say_something ()
{
    printf ("Say Something\n");
}

static int COUNTER = 0;

void set_counter (int count)
{
    COUNTER = count;
}

struct MyData
{
    float x;
};

PYBIND11_EMBEDDED_MODULE (aimWrapper, module)
{
    module.doc () = "aimWrapper Module";
    module.def ("say_something", &say_something);
    module.def ("set_counter", &set_counter);

    pybind11::class_<MyData>(module, "MyData")
    .def_readwrite ("x", &MyData::x);
}

int main ()
{
    pybind11::scoped_interpreter guard{};
    
    auto sys = pybind11::module::import ("sys");
    pybind11::print (sys.attr ("path"));
    MyData data {};
    data.x = 34.0f;
    pybind11::exec("import aimWrapper");
    pybind11::exec("aimWrapper.say_something()");
    std::cout<<data.x<<std::endl;
    printf ("end\n");
    return 0;
}
*/ 


#include <queue>
#include <fstream>
#include <iostream>
#include "aimwrapper.h"
#include "item.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>



namespace py = pybind11;

PYBIND11_MODULE(aimwrapper, m)
{
    py::class_ <WrapperClass>(m, "WrapperClass")
            .def(py::init())
            .def("startListening", &WrapperClass::startListening)
            .def("closeSocket", &WrapperClass::closeSocket);

    py::class_ <ItemString>(m, "AimStringWrapper")
            .def(py::init<> ())//const string &
            .def("startListeningString", &ItemString::startListeningString)
            .def("stopListeningString", &ItemString::stopListeningString)
            .def("getFlagPage", &ItemString::getFlagPage)
            .def("clearQueue", &ItemString::clearQueue)
            .def("clearAll", &ItemString::clearAll);
};




int main()
{
    
    int dataFlag;
    ItemString item;
    string size = "15000";
    item.startListeningString(size);
    while(1)
    {
        sleep(10000);
    }

    return 0;
}

