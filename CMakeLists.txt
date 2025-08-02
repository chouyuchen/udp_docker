cmake_minimum_required(VERSION 3.10)
project(udp_docker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add sub directories
add_subdirectory(server)
add_subdirectory(client)