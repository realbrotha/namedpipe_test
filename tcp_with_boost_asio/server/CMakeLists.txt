cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

set(MODULE_NAME asio_server)
set(SOURCES
        main.cpp)

include_directories(${INC_PATH})

add_executable(${MODULE_NAME} ${SOURCES})

target_link_libraries(${MODULE_NAME}
        asio_controller)

set_property(TARGET ${MODULE_NAME} PROPERTY OUTPUT_NAME ${MODULE_NAME})

install(
        TARGETS ${MODULE_NAME}
        RUNTIME DESTINATION ${BIN_PATH})