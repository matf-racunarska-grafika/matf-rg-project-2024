//
// Created by zesla on 10/9/25.
//

#include <../include/MyApp.hpp>
#include <GUIController.hpp>
#include <MainController.hpp>
#include <engine/graphics/DrawingController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <spdlog/spdlog.h>
namespace app {


    void app::MyApp::app_setup() {
        spdlog::info("App setup completed");
        auto main_controller = register_controller<app::MainController>();
        auto gui_controller = register_controller<app::GUIController>();
        auto drawing_controller = register_controller<engine::drawing::DrawingController>();
        drawing_controller->before(gui_controller);
        main_controller->before(gui_controller);
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    }
}// namespace app