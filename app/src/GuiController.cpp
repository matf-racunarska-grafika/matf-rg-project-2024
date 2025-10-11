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

    ImGui::SetNextWindowSize(ImVec2(400, 100));
    ImGui::Begin("Camera info", nullptr, ImGuiWindowFlags_NoResize);
    ImGui::Text("Camera Position: (%f, %f, %f)", camera->Position.x, camera->Position.y, camera->Position.z);
    ImGui::Text("Lamp Light Color:");
    ImGui::SameLine();
    ImGui::ColorEdit3("##LampLightColor", lampColor);
    ImGui::Text("Revolution Speed:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##RevolutionSpeed", &revolutionSpeed, 1, 10);
    revolutionSpeed = std::clamp(revolutionSpeed, 1, 20);
    ImGui::End();

    graphics->end_gui();
}
}// app