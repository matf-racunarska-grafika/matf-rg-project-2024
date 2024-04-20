cmake_minimum_required(VERSION 3.28)
project(matf_rg_engine)

set(CMAKE_CXX_STANDARD 20)

################ Libs ################
# spdlog
add_subdirectory(libs/spdlog)
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE SPDLOG_DEBUG_ON SPDLOG_TRACE_ON)


################ Exec ################
add_executable(matf_rg_engine main.cpp)
target_link_libraries(matf_rg_engine PRIVATE spdlog::spdlog)