//
// Created by matfrg on 7/14/25.
//

#include "../include/BedroomSimulation.hpp"

#include <../include/MainController.hpp>
#include <../../engine/include/engine/core/Controller.hpp>
#include <../../engine/libs/spdlog/include/spdlog/spdlog.h>

namespace app {
void BedroomSimulation::app_setup() {
    spdlog::info("App setup completed!");
    auto main_controller = register_controller<app::MainController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}
}// app