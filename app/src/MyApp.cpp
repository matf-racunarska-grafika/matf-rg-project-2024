//
// Created by filipn on 9/1/25.
//

#include <MainController.hpp>
#include <MyApp.hpp>

#include <spdlog/spdlog.h>

namespace app {
void MyApp::app_setup() {
    spdlog::info("App setup completed");
    auto main_controller = register_controller<app::MainController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}
}// app