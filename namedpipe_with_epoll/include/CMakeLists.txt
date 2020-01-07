cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME epoll_EpollNamedPipe)

set(SOURCES
        NamedPipeManager.cpp
        NamedPipeWrapper.cpp
        FileDescriptorTool.cpp
        StringTool.cpp)

add_library(${MODULE_NAME} STATIC ${SOURCES})
find_package(Threads REQUIRED)

target_link_libraries(${MODULE_NAME}
        ${CMAKE_THREAD_LIBS_INIT})
