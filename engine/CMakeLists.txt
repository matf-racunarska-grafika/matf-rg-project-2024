cmake_minimum_required(VERSION 3.21)
project(matf-rg-engine)
file(GLOB engine-sources src/*.cpp)
file(GLOB engine-headers include/engine/*.hpp)

add_library(${PROJECT_NAME} ${engine-sources} ${engine-headers})
target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw assimp ${ASSIMP_LIBRARIES} stb
        PUBLIC spdlog::spdlog glm imgui json)
