#include <spdlog/spdlog.h>
#include <engine/core/Engine.hpp>
#include "Main.h"
#include "MainController.h"

int main(int argc, char** argv) {
    auto app = std::make_unique<app::Main>();
    return app->run(argc, argv);
}

void app::Main::app_setup() {
        spdlog::info("app setup completed");
        auto main_controller = register_controller<MainController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}