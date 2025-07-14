
#include <GraveyardSimulation.hpp>
#include <engine/core/Engine.hpp>
/**
 * Start here...
 */
int main(int argc, char **argv) {
    auto app = std::make_unique<::app::GraveyardSimulation>();
    return app->run(argc, argv);
}
