
#include <BedroomSimulation.hpp>
#include <engine/core/Engine.hpp>
/**
 * Start here...
 */
int main(int argc, char **argv) {
    auto app = std::make_unique<::app::BedroomSimulation>();
    return app->run(argc, argv);
}
