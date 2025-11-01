#include <spdlog/spdlog.h>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/BloomController.h>
#include <engine/graphics/SpriteController.hpp>
#include "Main.h"
#include "MainController.h"
#include "GUIController.hpp"

int main(int argc, char** argv) {
    auto app = std::make_unique<app::Main>();
    return app->run(argc, argv);
}

void app::Main::app_setup() {
        spdlog::info("app setup completed");
        auto main_controller = register_controller<MainController>();
        auto gui_controller = register_controller<GUIController>();
        auto bloom = register_controller<engine::graphics::BloomController>();
        auto sprites = register_controller<engine::graphics::SpriteController>();

        // Bloom zavisi od Graphics i Resources (kontekst i šejderi)
        bloom->after(engine::core::Controller::get<engine::graphics::GraphicsController>());
        bloom->after(engine::core::Controller::get<engine::resources::ResourcesController>());
        sprites->after(engine::core::Controller::get<engine::graphics::GraphicsController>());
        sprites->after(engine::core::Controller::get<engine::resources::ResourcesController>());

        // Redosled: engine end -> bloom -> main -> gui
        main_controller->after(bloom);
        main_controller->after(sprites);
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
        gui_controller->after(main_controller);
}