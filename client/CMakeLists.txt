cmake_minimum_required(VERSION 3.10)
project(udp_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/udp_client.cpp
)

