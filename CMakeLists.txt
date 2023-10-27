cmake_minimum_required(VERSION 3.26)
project(Intel8080)

set(CMAKE_CXX_STANDARD 20)

add_library(Intel8080 STATIC
        src/Intel8080.cpp
        include/Intel8080.h
)

target_include_directories(Intel8080
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

add_executable(Intel8080_test
        src/Intel8080.test.cpp
)
target_link_libraries(Intel8080_test
        PRIVATE
        Intel8080
)