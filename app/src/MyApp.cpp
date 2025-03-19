//
// Created by strahinjas on 3/18/25.
//

#include <MainController.hpp>
#include <../include/MyApp.hpp>
#include <engine/core/Controller.hpp>

namespace my_project {
void MyApp::app_setup() {
    // Here I will register all the necessary controllers for my Application
    // -> Register my MainController after the engine controllers, for that I use the sentinel EngineControllersEnd

    auto main_controller = MyApp::register_controller<my_project::MyController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}
}