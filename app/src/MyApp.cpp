//
// Created by matfrg on 10/10/25.
//

#include "MyApp.hpp"

#include "GuiController.hpp"
#include "MainController.hpp"
#include "spdlog/spdlog.h"

namespace app {
    void MyApp::app_setup() {
        spdlog::info("App setup completed");
        auto main_controller = register_controller<app::MainController>();
        auto gui_controller = register_controller<app::GUIController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
        main_controller->before(gui_controller);

        gui_controller->set_main_controller(main_controller);
    }

} // app