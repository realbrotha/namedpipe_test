project(TestIpc)
cmake_minimum_required(VERSION 3.5)

add_subdirectory(namedpipe_with_epoll)
add_subdirectory(namedpipe_with_thread)
add_subdirectory(unixdomainsocket)
