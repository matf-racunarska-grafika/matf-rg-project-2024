#include <GuiController.hpp>
#include <MainController.hpp>
#include <PostProcessingController.hpp>
#include <MyApp.hpp>
#include <engine/core/Controller.hpp>
#include <spdlog/spdlog.h>

namespace app {
    void MyApp::app_setup() {
        spdlog::info("App setup completed!");
        auto main_controller = register_controller<app::MainController>();
        auto gui_controller = register_controller<app::GuiController>();
        auto post_processing_controller = register_controller<app::PostProcessingController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
        main_controller->before(gui_controller);
        post_processing_controller->after(main_controller);
    }
} // app