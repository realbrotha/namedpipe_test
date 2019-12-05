cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)

set(MODULE_NAME client)
set(SOURCES
        main.cpp)

add_executable(${MODULE_NAME} ${SOURCES})
set_property(TARGET ${MODULE_NAME} PROPERTY OUTPUT_NAME ${MODULE_NAME})

install(
        TARGETS ${MODULE_NAME}
        RUNTIME DESTINATION ${BIN_PATH})