cmake_minimum_required(VERSION 3.0)
project("AimWrapper")
set(CMAKE_CXX_STANDARD 20)

set(APP_SOURCE "${PROJECT_SOURCE_DIR}/source/app")
set(AIMWRAPPER "${PROJECT_SOURCE_DIR}/source/aimwrapper")
set(MODULE_SOURCE "${PROJECT_SOURCE_DIR}/source/module")
set(EXTERNALS "${PROJECT_SOURCE_DIR}/externals")

# pybind11
add_subdirectory(${EXTERNALS}/pybind11)
include_directories(${AIMWRAPPER})

# module
pybind11_add_module (
    aimwrapper 
    ${MODULE_SOURCE}/module.cpp
    ${AIMWRAPPER}/aimwrapper.cpp
    ${AIMWRAPPER}/item.cpp
    ${AIMWRAPPER}/wrapperClass.cpp
)

target_include_directories (
    aimwrapper 
    PUBLIC ${APP_SOURCE}
    PUBLIC ${MODULE_SOURCE}
    ${AIMWRAPPER}/aimwrapper.cpp
    ${AIMWRAPPER}/item.cpp
    ${AIMWRAPPER}/wrapperClass.cpp
)

# app
add_executable (
    pybind11app
    ${APP_SOURCE}/app.cpp
    ${AIMWRAPPER}/aimwrapper.cpp
    ${AIMWRAPPER}/item.cpp
    ${AIMWRAPPER}/wrapperClass.cpp
)

target_include_directories (
    pybind11app 
    PUBLIC ${APP_SOURCE}
    PUBLIC ${AIMWRAPPER}
    PUBLIC ${MODULE_SOURCE}
)

target_link_libraries(
    pybind11app
    PRIVATE pybind11::embed
)