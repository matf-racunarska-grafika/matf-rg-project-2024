cmake_minimum_required(VERSION 3.28)
project(matf_rg_engine)

set(CMAKE_CXX_STANDARD 20)

################ Libs ################
# OpenGL
find_package(OpenGL REQUIRED)

# spdlog
add_subdirectory(libs/spdlog)
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE SPDLOG_DEBUG_ON SPDLOG_TRACE_ON)

#glfw
add_subdirectory(libs/glfw)

# glad
add_subdirectory(libs/glad)

set(LIBS glfw glad OpenGL::GL spdlog::spdlog)

################ Exec ################
add_executable(matf_rg_engine main.cpp)
target_link_libraries(matf_rg_engine PRIVATE ${LIBS})