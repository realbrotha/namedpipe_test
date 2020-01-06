project(TestPipe)
cmake_minimum_required(VERSION 3.5)

set(BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/output_bin)
set(INC_PATH ${CMAKE_SOURCE_DIR}/include)
# include_directories(${INC_PATH})

add_subdirectory(include)
add_subdirectory(client)
add_subdirectory(server)
