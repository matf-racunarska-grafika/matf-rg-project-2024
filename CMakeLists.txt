cmake_minimum_required(VERSION 3.21)
project(matf-rg-project)

set(CMAKE_CXX_STANDARD 20)

################ Libs ################
# OpenGL
find_package(OpenGL REQUIRED)

# spdlog
add_subdirectory(libs/spdlog)
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE SPDLOG_DEBUG_ON SPDLOG_TRACE_ON RG_ENGINE_TRACE)

add_subdirectory(libs/glfw)
add_subdirectory(libs/glad)
include_directories(libs/json)
add_subdirectory(libs/glm)
add_subdirectory(libs/assimp)
add_subdirectory(libs/stb)

set(LIBS glfw glad OpenGL::GL stb spdlog::spdlog glm assimp ${ASSIMP_LIBRARIES})

############## Sources ###############

file(GLOB sources src/*.cpp engine/src/*.cpp)
file(GLOB headers src/*.hpp include/*.hpp engine/include/engine/*.hpp)

################ Exec ################
add_executable(${PROJECT_NAME} ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE engine/include include/)
add_dependencies(${PROJECT_NAME} ${LIBS})

# Define the source and destination directories
set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/resources/shaders")
set(SHADERS_DEST_DIR "${CMAKE_BINARY_DIR}/resources/shaders")

# Add a custom target to copy resources
add_custom_target(copy-shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADERS_DIR} ${SHADERS_DEST_DIR}
        COMMENT "Copying resources from ${SHADERS_DIR} to ${SHADERS_DEST_DIR}"
)

add_dependencies(matf-rg-project copy-shaders)
