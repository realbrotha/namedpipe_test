cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME asio_controller)

set(SOURCES
        AsioClient.cpp
        AsioServer.cpp
        AsioFactory.h
        AsioInterface.cpp
        FileDescriptorTool.cpp
        StringTool.cpp)

include_directories(${INC_PATH})

add_library(${MODULE_NAME} STATIC ${SOURCES})
find_package(Threads REQUIRED)

target_link_libraries(${MODULE_NAME}
        ${CMAKE_THREAD_LIBS_INIT})
