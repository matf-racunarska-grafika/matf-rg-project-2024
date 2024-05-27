cmake_minimum_required(VERSION 3.11)

############## Sources ###############
file(GLOB sources src/*.cpp)
file(GLOB headers include/engine/*.hpp include/engine/*.h)

add_library(rg-engine ${sources} ${headers})

target_include_directories(rg-engine PUBLIC include/)
set(LIBS glfw glad OpenGL::GL spdlog::spdlog nlohmann_json::nlohmann_json)
target_link_libraries(rg-engine ${LIBS})
add_dependencies(rg-engine ${LIBS})
add_custom_target(rg-engine-format COMMAND clang-format -i ${sources} ${headers})
