//
// Created by matija on 10/12/25.
//

#include "../include/MyApp.hpp"

#include <MySceneController.hpp>

namespace app {
void MyApp::app_setup() {
    auto main_controller = register_controller<MySceneController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());


}
}