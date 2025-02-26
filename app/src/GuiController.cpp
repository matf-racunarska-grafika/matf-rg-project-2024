//
// Created by masa on 30/01/25.
//

#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>

#include "../include/GuiController.hpp"

#include <MainController.hpp>
#include <imgui.h>

#include <MainController.hpp>

namespace app {
    void GUIController::initialize() {
        set_enable(false);
    }

    void GUIController::poll_events() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_H).state() == engine::platform::Key::State::JustPressed) {
            set_enable(!is_enabled());
        }
    }

    void GUIController::draw() {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->begin_gui();


        ImGui::Begin("ImGui");
        ImGui::DragFloat("radius of butterflies", &radius, 1, 1.0f, 50.0f, "%f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::DragFloat("gazebo light", &light_gazebo, 1, 1.0f, 150.0f, "%f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::End();

        graphics->end_gui();
    }

} // namespace app
