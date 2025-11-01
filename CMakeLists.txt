cmake_minimum_required(VERSION 3.21)
project(mesec_nebo_zvezdice)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/find_python.cmake)
find_package(Python3 COMPONENTS Interpreter Development)
include(cmake/functions.cmake)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)
if (NOT COMPILER_SUPPORTS_CXX20)
    message(FATAL_ERROR "The compiler does not support C++20.")
endif ()


############## ENGINE  ###############
add_subdirectory(engine EXCLUDE_FROM_ALL)

############# TEST ###############
option(BUILD_TEST_APP "Builds a test application" ON)
if (BUILD_TEST_APP)
    add_subdirectory(engine/test/app)
endif ()

############ APP #################
option(BUILD_APP "Builds the app" ON)
if (BUILD_APP)
    add_subdirectory(app/)
endif ()