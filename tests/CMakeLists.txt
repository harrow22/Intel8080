add_executable(Intel8080_test
        Intel8080.test.cpp
)

target_link_libraries(Intel8080_test
        PRIVATE
        Intel8080
)