cmake_minimum_required(VERSION 3.11)
project(APP)
file(GLOB sources src/*.cpp)
file(GLOB headers include/*.h)

include_directories(include/)

add_executable(${PROJECT_NAME} ${sources} ${headers}
        src/MainController.cpp
        src/MainController.cpp
        include/MainController.h
        include/Main.h)

target_link_libraries(${PROJECT_NAME} PRIVATE matf-rg-engine)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
prebuild_check(${PROJECT_NAME})