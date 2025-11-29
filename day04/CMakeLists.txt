cmake_minimum_required(VERSION 3.25)

project(WebServer)

aux_source_directory(src SRC_LIST)

include_directories(src)

add_executable(server server.cpp ${SRC_LIST})
add_executable(client client.cpp ${SRC_LIST})