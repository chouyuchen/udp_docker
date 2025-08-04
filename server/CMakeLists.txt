cmake_minimum_required(VERSION 3.10)
project(udp_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# source files for the server
add_library(udp_server_lib 
    src/udp_server.cpp
)

# include directories for the library (header files)
target_include_directories(udp_server_lib 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# for the main executable
add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        udp_server_lib
)

# for testing
include(FetchContent)
FetchContent_Declare(
  googletest
  URL  https://github.com/google/googletest/archive/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# add test executable files
add_executable(test_${PROJECT_NAME}
    test/test_server.cpp
)

# target_include_directories(test_${PROJECT_NAME}
#     PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc
# )

target_link_libraries(test_${PROJECT_NAME}
    PRIVATE
        udp_server_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_${PROJECT_NAME})