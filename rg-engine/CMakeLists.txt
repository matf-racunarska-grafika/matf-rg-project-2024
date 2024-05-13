cmake_minimum_required(VERSION 3.11)

############## Sources ###############
file(GLOB sources src/*.cpp)
file(GLOB headers include/engine/*.hpp include/engine/*.h)

add_library(rg-engine ${sources} ${headers})

target_include_directories(rg-engine PUBLIC include/)
target_link_libraries(rg-engine glfw glad OpenGL::GL spdlog::spdlog)
add_dependencies(rg-engine glfw glad OpenGL::GL spdlog::spdlog)
add_custom_target(rg-engine-format COMMAND clang-format -i ${sources} ${headers})
