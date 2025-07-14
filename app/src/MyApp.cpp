//
// Created by cvnpko on 7/14/25.
//

#include <MyApp.hpp>
#include <engine/core/Controller.hpp>
#include <MainController.hpp>

namespace app {
class MainController;

void MyApp::app_setup() {
    auto main_controller = register_controller<MainController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}
}// app