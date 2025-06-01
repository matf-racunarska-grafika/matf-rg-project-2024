//
// Created by aleksa on 30.5.25..
//

#include "MyApp.h"

#include "MainController.h"
#include "spdlog/spdlog.h"

namespace app {
    void MyApp::app_setup() {
        spdlog::info("App setup");

        auto main_controller = register_controller<app::MainController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    }
} // app