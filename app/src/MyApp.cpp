//
// Created by nikola on 10/8/25.
//

#include <MyApp.hpp>

#include "spdlog/spdlog.h"
#include <MyController.hpp>

namespace app {

    void MyApp::app_setup() {
        spdlog::info("App setup completed!");
        auto main_controller = register_controller<MyController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    }

}// namespace app