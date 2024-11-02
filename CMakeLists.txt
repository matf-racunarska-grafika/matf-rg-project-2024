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
# rg-include
add_subdirectory(rg-engine)

set(LIBS glfw glad OpenGL::GL spdlog::spdlog rg-engine)

############## Sources ###############

file(GLOB sources src/*.cpp)
file(GLOB headers include/*.hpp)

################ Exec ################
add_executable(${PROJECT_NAME} ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
add_dependencies(${PROJECT_NAME} ${LIBS})

add_custom_target(matf-rg-project-format COMMAND clang-format -i ${sources} ${headers})

################ Copy rg-engine-resources #################
# Define source and destination directories
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/rg-engine/resources")
set(DEST_DIR "${CMAKE_BINARY_DIR}/cmake-build-debug")

# Collect all resource files
file(GLOB_RECURSE RESOURCE_FILES "${RESOURCE_DIR}/*")

# Create a custom target to copy files
add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_DIR} ${DEST_DIR}
        COMMENT "Copying resources to build directory..."
)

# Set up a dependency on the resource files so that files are copied when they change
foreach (RESOURCE_FILE ${RESOURCE_FILES})
    add_custom_command(TARGET copy_resources
            DEPENDS ${RESOURCE_FILE}
    )
endforeach ()