//
// Created by cvnpko on 7/14/25.
//

#include <MainEventController.hpp>
#include <MyApp.hpp>
#include <engine/core/Controller.hpp>
#include <MainController.hpp>

namespace app {
class MainController;

void MyApp::app_setup() {
    auto main_event_controller = register_controller<MainEventController>();
    main_event_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    auto main_controller = register_controller<MainController>();
    main_controller->after(main_event_controller);
}
}// app