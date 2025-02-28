#include <engine/core/Controller.hpp>
#include <engine/graphics/PostProcessingController.hpp>
#include <GuiController.hpp>
#include <MainController.hpp>
#include <MyApp.hpp>

#include <spdlog/spdlog.h>

namespace app {
    void MyApp::app_setup() {
        spdlog::info("App setup completed!");
        auto main_controller = register_controller<app::MainController>();
        auto gui_controller = register_controller<app::GuiController>();
        auto ppc = engine::core::Controller::get<engine::graphics::PostProcessingController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
        ppc->after(main_controller);
        gui_controller->after(ppc);
    }
} // app