//
// Created by nikola on 10/8/25.
//


#include <MyApp.hpp>

#include "spdlog/spdlog.h"

#include <BloomController.hpp>
#include <MyController.hpp>
#include <GUIController.hpp>

namespace app {

    void MyApp::app_setup() {
        spdlog::info("App setup completed!");
        auto main_controller = register_controller<MyController>();
        auto gui_controller = register_controller<GUIController>();
        auto bloom_controller = register_controller<BloomController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
        gui_controller->after(main_controller);
        bloom_controller->after(gui_controller);
    }

}// namespace app