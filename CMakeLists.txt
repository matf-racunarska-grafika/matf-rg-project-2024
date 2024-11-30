cmake_minimum_required(VERSION 3.21)
project(matf-rg-project)

set(CMAKE_CXX_STANDARD 20)

################ Libs ################
# OpenGL
find_package(OpenGL REQUIRED)

# spdlog
add_subdirectory(libs/spdlog)
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE SPDLOG_DEBUG_ON SPDLOG_TRACE_ON RG_ENGINE_TRACE)

#glfw
add_subdirectory(libs/glfw)

# glad
add_subdirectory(libs/glad)

# json
include_directories(libs/json)
add_subdirectory(libs/glm)

#assimp
add_subdirectory(libs/assimp)

add_subdirectory(libs/stb)

# rg-include
add_subdirectory(rg-engine)

set(LIBS glfw glad OpenGL::GL spdlog::spdlog glm rg-engine ${ASSIMP_LIBRARIES})

############## Sources ###############

file(GLOB sources src/*.cpp)
file(GLOB headers include/*.hpp)

################ Exec ################
add_executable(${PROJECT_NAME} ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
add_dependencies(${PROJECT_NAME} ${LIBS})

# Define the source and destination directories
set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/src/shaders")
set(SHADERS_DEST_DIR "${CMAKE_BINARY_DIR}/resources/shaders")

# Add a custom target to copy resources
add_custom_target(copy-shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADERS_DIR} ${SHADERS_DEST_DIR}
        COMMENT "Copying resources from ${SHADERS_DIR} to ${SHADERS_DEST_DIR}"
)

add_dependencies(matf-rg-project copy-shaders)
