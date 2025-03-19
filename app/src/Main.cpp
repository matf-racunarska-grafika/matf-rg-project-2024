#include <engine/core/Engine.hpp>
#include <MyApp.hpp>
/**
 * Start here...
 */

int main(int argc, char **argv) { return std::make_unique<my_project::MyApp>()->run(argc, argv); }
