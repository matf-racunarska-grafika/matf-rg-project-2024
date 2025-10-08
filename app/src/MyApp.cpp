//
// Created by bojana on 10/8/25.
//

#include <MyApp.hpp>
#include <MyController.hpp>
#include <GUIController.hpp>
#include <engine/core/Controller.hpp>
#include <spdlog/spdlog.h>
namespace app {
void MyApp::app_setup() {
    spdlog::info("App setup");
    auto main_controller = register_controller<MyController>();
    auto gui_controller = register_controller<GUIController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    main_controller->after(main_controller);
}
}