cmake_minimum_required(VERSION 3.21)
project(matf-rg-project)

set(CMAKE_CXX_STANDARD 23)

################ Libs ################
add_subdirectory(libs/spdlog EXCLUDE_FROM_ALL)
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE SPDLOG_DEBUG_ON SPDLOG_TRACE_ON RG_ENGINE_TRACE)

add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)
add_subdirectory(libs/glad EXCLUDE_FROM_ALL)
add_subdirectory(libs/json EXCLUDE_FROM_ALL)
add_subdirectory(libs/glm EXCLUDE_FROM_ALL)
add_subdirectory(libs/assimp EXCLUDE_FROM_ALL)
add_subdirectory(libs/stb EXCLUDE_FROM_ALL)
add_subdirectory(libs/imgui EXCLUDE_FROM_ALL)

############## ENGINE  ###############

add_subdirectory(engine/ EXCLUDE_FROM_ALL)

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