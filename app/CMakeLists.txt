cmake_minimum_required(VERSION 3.11)
project(APP)

# 1) C++ sources and headers
file(GLOB sources src/*.cpp)
file(GLOB headers include/*.hpp)

# 2) Shader files: .vert, .frag, and .glsl
file(GLOB shader_sources
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.{vert,frag,glsl}"
)

# 3) Include path for your own headers
include_directories(include/)

# 4) Build the executable, now including shaders
add_executable(${PROJECT_NAME}
        ${sources}
        ${headers}
        ${shader_sources}    # <- shaders will be indexed by CLion
)

# 5) Link against your engine
target_link_libraries(${PROJECT_NAME} PRIVATE matf-rg-engine)

# 6) Use C++20
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# 7) Place binary next to CMakeLists.txt
set_target_properties(${PROJECT_NAME}
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

prebuild_check(${PROJECT_NAME})
