cmake_minimum_required(VERSION 3.25)

project(WebServer)

include_directories(
    base 
    tcp
    http
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

aux_source_directory(base SRC_LIST1)
aux_source_directory(tcp SRC_LIST2)
aux_source_directory(http SRC_LIST3)

add_executable(server ./test/server.cpp ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3})
add_executable(client ./test/client.cpp ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3})