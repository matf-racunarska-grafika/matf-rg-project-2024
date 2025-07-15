//
// Created by matfrg on 7/14/25.
//

#include "../include/GraveyardSimulation.hpp"

#include <GuiController.hpp>
#include <../include/MainController.hpp>
#include <../../engine/include/engine/core/Controller.hpp>
#include <../../engine/libs/spdlog/include/spdlog/spdlog.h>

namespace app {
void GraveyardSimulation::app_setup() {
    spdlog::info("App setup completed!");
    auto main_controller = register_controller<app::MainController>();
    auto gui_controller = register_controller<app::GUIController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    main_controller->before(gui_controller);
}
}// app