//
// Created by filip on 1/22/25.
//

#include <myApp.hpp>
#include <spdlog/spdlog.h>

#include "MainController.hpp"

namespace app{
    void myApp::app_setup(){
        spdlog::info("myApp::app_setup()");
        auto main_controller = register_controller<app::MainController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    }
}