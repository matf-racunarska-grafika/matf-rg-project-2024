cmake_minimum_required(VERSION 3.11)

set(MAIN_APP APP)
file(GLOB sources src/*.cpp)
file(GLOB headers include/*.hpp)

include_directories(include/)
add_executable(${MAIN_APP} ${sources} ${headers})
target_link_libraries(${MAIN_APP} PRIVATE matf-rg-engine)
target_compile_features(${MAIN_APP} PRIVATE cxx_std_20)
set_target_properties(${MAIN_APP} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
prebuild_check(${MAIN_APP})