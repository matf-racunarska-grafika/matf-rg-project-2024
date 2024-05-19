cmake_minimum_required(VERSION 3.11)

add_library(glad glad.cpp include/glad/glad.h)

target_include_directories(glad PUBLIC include/)