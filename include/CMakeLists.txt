cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME named_pipe)

set(SOURCES
        NamedPipeServer.cpp
        NamedPipeWrapper.cpp)

add_library(${MODULE_NAME} STATIC ${SOURCES})
find_package(Threads REQUIRED)

target_link_libraries(${MODULE_NAME}
        ${CMAKE_THREAD_LIBS_INIT})