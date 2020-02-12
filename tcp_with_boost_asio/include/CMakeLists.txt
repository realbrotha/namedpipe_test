cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME asio_controller)

#set(INC_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${INC_PATH})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(SOURCES
        AsioClient.cpp
        AsioServer.cpp
        AsioFactory.h
        AsioInterface.cpp
        FileDescriptorTool.cpp
        StringTool.cpp
        AsioImpl.hpp)

include_directories(${INC_PATH})

#find_package(Boost COMPONENTS system filesystem REQUIRED)

add_library(${MODULE_NAME} STATIC ${SOURCES})
find_package(Threads REQUIRED)

target_link_libraries(${MODULE_NAME}
        ${CMAKE_THREAD_LIBS_INIT}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libboost_system.a
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libboost_thread.a)
