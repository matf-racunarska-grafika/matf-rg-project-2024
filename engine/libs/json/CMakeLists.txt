cmake_minimum_required(VERSION 3.21)

add_library(json INTERFACE)
target_include_directories(json INTERFACE INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)