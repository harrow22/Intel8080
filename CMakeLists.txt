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

option(INTEL8080_TESTS "Enable / Disable testing" ON)
if (INTEL8080_TESTS)
    add_subdirectory(tests)
endif()