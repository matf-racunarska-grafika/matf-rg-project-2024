cmake_minimum_required(VERSION 3.11)

############## Sources ###############
file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE headers include/engine/*.hpp include/engine/*.h)

add_library(rg-engine ${sources} ${headers})


target_include_directories(rg-engine PUBLIC include/)
set(LIBS glfw glad OpenGL::GL spdlog::spdlog glm assimp stb ${ASSIMP_LIBRARIES})
target_link_libraries(rg-engine ${LIBS})
add_dependencies(rg-engine ${LIBS})
add_custom_target(rg-engine-format COMMAND clang-format -i ${sources} ${headers})
