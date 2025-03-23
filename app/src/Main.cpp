#include <engine/core/Engine.hpp>
#include <MyApp.hpp>
#include <iostream>
/**
 * Start here...
 */

int main(int argc, char **argv) {
    std::cout << "Welcome to my application | Floating safe haven |" << std::endl;
    return std::make_unique<my_project::MyApp>()->run(argc, argv);
}
