cmake_minimum_required(VERSION 3.18)
project(http C)

set(CMAKE_C_STANDARD 99)
configure_file(temp.json temp.json COPYONLY)
add_executable(http main.c)
