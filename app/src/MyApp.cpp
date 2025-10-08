//
// Created by bojana on 10/8/25.
//

#include <MyApp.hpp>
#include <MyController.hpp>
#include <engine/core/Controller.hpp>
#include <spdlog/spdlog.h>
namespace app {
void MyApp::app_setup() {
    spdlog::info("App setup");
    auto main_controller = register_controller<MyController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}
}