cmake_minimum_required(VERSION 3.21)
project(matf-rg-project)

set(CMAKE_CXX_STANDARD 20)

################ Libs ################
add_subdirectory(libs/spdlog)
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE SPDLOG_DEBUG_ON SPDLOG_TRACE_ON RG_ENGINE_TRACE)

add_subdirectory(libs/glfw)
add_subdirectory(libs/glad)
include_directories(libs/json)
add_subdirectory(libs/glm)
add_subdirectory(libs/assimp)
add_subdirectory(libs/stb)
add_subdirectory(libs/imgui)

############## ENGINE  ###############
file(GLOB engine-sources engine/src/*.cpp)
file(GLOB engine-headers engine/include/engine/*.hpp)

add_library(engine ${engine-sources} ${engine-headers})
target_include_directories(engine PUBLIC engine/include)
target_link_libraries(engine PRIVATE glad glfw assimp ${ASSIMP_LIBRARIES} stb
        PUBLIC spdlog::spdlog glm imgui)

############# TEST ###############
option(BUILD_TEST_APP "Builds a test application" ON)
if (BUILD_TEST_APP)
    add_subdirectory(engine/test/app)
endif ()