//
// Created by masa on 28/01/25.
//

#include "../include/MyApp.hpp"

#include <GuiController.hpp>
#include <MainController.hpp>
#include <spdlog/spdlog.h>

namespace app {
    void MyApp::app_setup() {
        spdlog::info("App setup completed!");
        auto main_controller = register_controller<MainController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
        auto gui_controller = register_controller<app::GUIController>();
        main_controller->before(gui_controller);
    }

} // app