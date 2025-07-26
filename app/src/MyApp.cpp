//
// Created by boce on 7/26/25.
//


#include <MyApp.hpp>
#include <MainController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void MyApp::app_setup() {
    spdlog::info("app setup!");
    auto main_controller = register_controller<app::MainController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}
}// app