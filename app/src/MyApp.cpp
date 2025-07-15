//
// Created by ognje on 7/14/2025.
//

#include <GuiController.hpp>
#include <MainController.hpp>
#include <MyApp.hpp>

#include <spdlog/spdlog.h>

namespace app {
    void MyApp::app_setup() {
        spdlog::info("App setup completed");
        auto main_controller = register_controller<app::MainController>();
        auto gui_controller = register_controller<GUIController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
        main_controller->before(gui_controller);
    }
}// app