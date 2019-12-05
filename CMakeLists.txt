cmake_minimum_required(VERSION 3.10)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

add_executable(untitled main.cpp IpcServer.cpp IpcServer.h named_pipe_wrapper.cpp named_pipe_wrapper.h)