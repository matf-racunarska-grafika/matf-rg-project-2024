//
// Created by zesla on 10/25/25.
//

#include "../include/GUIController.hpp"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <imgui.h>
extern bool g_lamp_is_on;
extern bool g_lamp_is_flickering;
extern bool g_start_flicker_sequence;
extern float g_flicker_timer;


namespace app {

std::string_view GUIController::name() const {
    return Controller::name();
}
void GUIController::initialize() {

    set_enable(false);
}
void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    graphics->begin_gui();

    // Draw camera info
    ImGui::Begin("Camera info");
    const auto &c = *camera;
    ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
    ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
    ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
    ImGui::End();

    ImGui::Begin("Lamp Control");

    ImGui::Text("Lamp Status: %s", g_lamp_is_on ? "ON" : "OFF");

    if (g_lamp_is_flickering) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "FLICKERING...");
        ImGui::ProgressBar(g_flicker_timer / 3.0f, ImVec2(0.0f, 0.0f));
    }

    ImGui::Separator();

    if (ImGui::Button("Trigger Flicker Effect", ImVec2(200, 30))) {
        g_start_flicker_sequence = true;
    }

    ImGui::TextWrapped("Flicker effect: Waits 3s, flickers for 3s, then randomly turns on/off");

    ImGui::End();

    graphics->end_gui();
}

void GUIController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_F2).state() == engine::platform::Key::State::JustPressed)
    {
        set_enable(!is_enabled());
    }

}
}// namespace app