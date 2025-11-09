#include <MyApp.hpp>

#include <engine/core/Engine.hpp>


int main(int argc, char** argv) {
    return std::make_unique<app::MyApp>()->run(argc, argv);
}
