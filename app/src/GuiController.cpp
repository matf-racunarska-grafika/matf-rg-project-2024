//
// Created by matfrg on 10/13/25.
//

#include "GuiController.hpp"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <imgui.h>

namespace app {

void GUIController::initialize() {
    set_enable(false);
}
void GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if(platform->key(engine::platform::KeyId::KEY_F2).state()==engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
        if(is_enabled()) {
            platform->set_enable_cursor(true);
        }else {
            platform->set_enable_cursor(false);
        }
    }
}
void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    graphics->begin_gui();
    ImGui::Begin("Camera info");
    ImGui::Text("Pozicija kamere (%.2f,%.2f,%.2f)",camera->Position.x,camera->Position.y,camera->Position.z);


    ImGui::End();

    graphics->end_gui();

}

} // app