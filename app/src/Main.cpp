#include <engine/core/Engine.hpp>

#include <myApp.hpp>
/**
 * Start here...
 */
int main(int argc, char** argv) {
    auto app = std::make_unique<app::myApp>();
    return app->run(argc, argv);
}
