cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(MODULE_NAME UnixDomainSocket)

set(SOURCES
        SocketWrapper.cpp
        EpollWrapper.cpp
        UnixDomainSocketFactory.h
        Message.cpp
        UnixDomainSocketSessionManager.cpp
        UnixDomainSocketClient.cpp
        UnixDomainSocketServer.cpp
        UnixDomainSocketInterface.cpp
        FileDescriptorTool.cpp
        StringTool.cpp)

add_library(${MODULE_NAME} STATIC ${SOURCES})
find_package(Threads REQUIRED)

target_link_libraries(${MODULE_NAME}
        ${CMAKE_THREAD_LIBS_INIT})
