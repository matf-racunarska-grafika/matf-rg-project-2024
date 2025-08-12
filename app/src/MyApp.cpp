//
// Created by aleksa on 30.5.25..
//

#include "MyApp.h"

#include "MainController.h"
#include "spdlog/spdlog.h"
#include <GuiController.hpp>

namespace app {
void MyApp::app_setup() {
    spdlog::info("App setup");

    auto main_controller = register_controller<app::MainController>();
    auto gui_controller = register_controller<app::GUIController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    main_controller->before(gui_controller);
}
}// app