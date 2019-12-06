cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

set(MODULE_NAME server)
set(SOURCES
        main.cpp IpcServer.cpp named_pipe_wrapper.cpp named_pipe_manager.cpp)

find_package(Threads REQUIRED)
add_executable(${MODULE_NAME} ${SOURCES})
target_link_libraries(${MODULE_NAME} ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET ${MODULE_NAME} PROPERTY OUTPUT_NAME ${MODULE_NAME})

install(
        TARGETS ${MODULE_NAME}
        RUNTIME DESTINATION ${BIN_PATH})