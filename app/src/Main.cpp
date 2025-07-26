#include <engine/core/Engine.hpp>

#include <MyApp.hpp>

/**
 * Start here...
 */
int main(int argc, char **argv) {
    auto app = std::make_unique<app::MyApp>();
    return app->run(argc, argv);
}
