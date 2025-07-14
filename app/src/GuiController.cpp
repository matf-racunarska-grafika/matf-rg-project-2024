//
// Created by Jovana on 14.7.2025..
//

#include <GuiController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <imgui.h>

namespace app{

void GUIController::initialize(){
    set_enable(false);
}

void GUIController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_F2)
                .state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
                }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    graphics->begin_gui();

    ImGui::Begin("Lighting Controls");

    ImGui::Separator();
    ImGui::Text("Point Light Settings");
    ImGui::SliderFloat("Point Light Intensity", &pointLightIntensity, 0.0f, 5.0f);
    ImGui::ColorEdit3("Point Light Color", (float*)&pointLightColor);

    ImGui::Separator();
    ImGui::Text("Directional Light Settings");
    ImGui::SliderFloat("Directional Light Intensity", &dirLightIntensity, 0.0f, 5.0f);

    ImGui::Separator();
    ImGui::Text("Camera Info");
    ImGui::Text("Position: (%.1f, %.1f, %.1f)", camera->Position.x, camera->Position.y, camera->Position.z);
    ImGui::Text("Yaw: %.1f, Pitch: %.1f", camera->Yaw, camera->Pitch);

    ImGui::Separator();
    ImGui::Text("Debug Info");
    ImGui::Text("Use F2 to toggle this menu.");

    ImGui::End();

    graphics->end_gui();
}


}