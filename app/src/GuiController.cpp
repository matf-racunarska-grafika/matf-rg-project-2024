//
// Created by filipn on 9/2/25.
//

#include <GuiController.hpp>
#include <imgui.h>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>

namespace app {
void GuiController::initialize() { set_enable(false); }

void GuiController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_M).state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
        platform->set_enable_cursor(is_enabled());
    }
}

void GuiController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    graphics->begin_gui();

    ImGui::Begin("Camera info");
    ImGui::Text("Camera Position: (%f, %f, %f)", camera->Position.x, camera->Position.y, camera->Position.z);
    ImGui::ColorPicker3("Lamp Light Color: ", lampColor);
    ImGui::End();

    graphics->end_gui();
}
}// app