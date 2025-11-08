//
// Created by zesla on 10/9/25.
//

#include <MyApp.hpp>
#include <GUIController.hpp>
#include <MainController.hpp>
#include <engine/graphics/DrawingController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/VisionController.hpp>
#include <spdlog/spdlog.h>
namespace app {


    void app::MyApp::app_setup() {
        spdlog::info("App setup completed");
        auto main_controller = register_controller<app::MainController>();
        auto gui_controller = register_controller<app::GUIController>();
        auto drawing_controller = register_controller<engine::drawing::DrawingController>();
        auto resources = register_controller<engine::resources::ResourcesController>();
        auto vision = register_controller<engine::vision::VisionController>();

        drawing_controller->set_enable(false);

        drawing_controller->before(gui_controller);
        resources->before(vision);
        main_controller->before(gui_controller);
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    }
}// namespace app