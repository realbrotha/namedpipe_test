cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME EpollNamedPipe)

set(SOURCES
        EpollHandler.cpp
        FileDescriptorTool.cpp
        StringTool.cpp)

add_library(${MODULE_NAME} STATIC ${SOURCES})

target_link_libraries(${MODULE_NAME})