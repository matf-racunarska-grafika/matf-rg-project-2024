//
// Created by teodora on 4.7.25..
//

#include "GuiController.hpp"

#include "engine/graphics/GraphicsController.hpp"
#include "engine/platform/PlatformController.hpp"
#include "imgui.h"

namespace app {
void app::GUIController::initialize() {
    set_enable(false);
}

void GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
    }
}

void app::GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    graphics->begin_gui();

    ImGui::Begin("Camera info");
    ImGui::Text("Camera position: (%f, %f, %f)", camera->Position.x, camera->Position.y, camera->Position.z);
    ImGui::Text("(Yaw, Pitch): (%f, %f)", camera->Yaw, camera->Pitch);
    ImGui::Text("Camera front: (%f, %f, %f)", camera->Front.x, camera->Front.y, camera->Front.z);

    ImGui::End();
    graphics->end_gui();
}
}// namespace app