cmake_minimum_required(VERSION 3.1.0)

project(helloworld)

add_executable(helloworld
    test.cpp
)

target_link_libraries(helloworld pthread)
