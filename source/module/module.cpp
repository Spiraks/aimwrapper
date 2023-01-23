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