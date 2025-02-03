#include <engine/core/Controller.hpp>

#include <GUIController.hpp>
#include <MainController.hpp>
#include <MyApp.hpp>
#include <spdlog/spdlog.h>

namespace app {
    void MyApp::app_setup() {
        spdlog::info("App setup completed");
        auto main_controller = register_controller<app::MainController>();
        auto gui_controller  = register_controller<GUIController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
        gui_controller->after(main_controller);
    }

}