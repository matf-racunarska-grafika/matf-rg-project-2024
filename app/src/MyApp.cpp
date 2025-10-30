//
// Created by matija on 10/12/25.
//

#include "../include/MyApp.hpp"

#include <MyGUIController.hpp>
#include <MySceneController.hpp>

namespace app {
void MyApp::app_setup() {
    auto main_controller = register_controller<MySceneController>();
    auto gui_controller = register_controller<MyGUIController>();

    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    gui_controller->after(main_controller);

}
}