#include <engine/core/Engine.hpp>

/**
 * Start here...
 */
#include <MyApp.hpp>

#include <MainController.hpp>
int main(int argc, char **argv) {
    return std::make_unique<my_project::MyApp>()->run(argc, argv);
}
