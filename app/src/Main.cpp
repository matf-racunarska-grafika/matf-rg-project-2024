#include <MyApp.hpp>
#include <spdlog/spdlog.h>
/**
 * Start here...
 */

int main(int argc, char **argv) {
    spdlog::info("Welcome to my application | Floating safe Haven |");
    return std::make_unique<my_project::MyApp>()->run(argc, argv);
}
