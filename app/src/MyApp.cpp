//
// Created by matfrg on 11/1/25.
//

#include <MyApp.hpp>
#include <MainController.hpp>
#include <../include/MyApp.hpp>
#include <engine/core/Controller.hpp>

namespace my_project {
void MyApp::app_setup() {

    auto main_controller = MyApp::register_controller<my_project::MyController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}
}
