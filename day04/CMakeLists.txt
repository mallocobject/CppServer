cmake_minimum_required(VERSION 3.25)

project(WebServer)

aux_source_directory(src SRC_LIST)

include_directories(src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(server server.cpp ${SRC_LIST})
add_executable(client client.cpp ${SRC_LIST})
# add_executable(test test.cpp ${SRC_LIST})