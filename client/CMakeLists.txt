cmake_minimum_required(VERSION 3.10)
project(udp_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(udp_client_lib 
    src/udp_client.cpp
)

target_include_directories(udp_client_lib 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        udp_client_lib
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL  https://github.com/google/googletest/archive/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_${PROJECT_NAME}
    test/test_client.cpp
)

target_link_libraries(test_${PROJECT_NAME}
    PRIVATE
        udp_client_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_${PROJECT_NAME})