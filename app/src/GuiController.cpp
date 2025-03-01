//
// Created by masa on 30/01/25.
//

#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>

#include "../include/GuiController.hpp"

#include <MainController.hpp>
#include <imgui.h>

#include <MainController.hpp>
#include <glm/glm.hpp>

namespace app {
    void GUIController::initialize() {
        set_enable(false);
    }

    void GUIController::poll_events() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_H).state() == engine::platform::Key::State::JustPressed) {
            set_enable(!is_enabled());
            m_enabled = !m_enabled;
            platform->set_enable_cursor(m_enabled);
        }
    }

    void GUIController::updateLampColors() {
        for (int i = 0; i < 5; i++) {
            lampColors[i] = baseColors[i] * brightness[i];
        }
    }

    void GUIController::draw() {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->begin_gui();

        ImGui::Begin("ImGui");

        ImGuiStyle &style = ImGui::GetStyle();
        ImVec4 *colors    = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.245f, 0.173f, 0.210f, 0.75f);
        colors[ImGuiCol_Button]   = ImVec4(0.173f, 0.216f, 0.230f, 1.0f);

        style.FrameRounding = 4.0f;
        style.FramePadding  = ImVec2(8, 4);

        ImGui::Text("Change the intensity of the light in gazebo and see what happens!!!");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::DragFloat("gazebo light", &light_gazebo, 1, 1.0f, 150.0f, "%f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::Spacing();
        ImGui::Separator();
        if (light_gazebo > 50.0f)
            ImGui::DragFloat("radius of butterflies", &radius, 1, 1.0f, 50.0f, "%f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::Spacing();
        ImGui::Separator();

        static bool alpha_preview      = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop      = true;
        static bool options_menu       = true;
        static bool hdr                = false;

        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) |
                                         (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) |
                                         (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf
                                                             : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) |
                                         (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

        ImGui::Text("Change colors of the lamp post lights");

        bool colorsChanged = false;

        colorsChanged |= ImGui::ColorEdit3("Base Color 1", (float *) &baseColors[0], misc_flags);
        colorsChanged |= ImGui::ColorEdit3("Base Color 2", (float *) &baseColors[1], misc_flags);
        colorsChanged |= ImGui::ColorEdit3("Base Color 3", (float *) &baseColors[2], misc_flags);
        colorsChanged |= ImGui::ColorEdit3("Base Color 4", (float *) &baseColors[3], misc_flags);
        colorsChanged |= ImGui::ColorEdit3("Base Color 5", (float *) &baseColors[4], misc_flags);

        ImGui::Spacing();
        ImGui::Text("Adjust brightness");

        const float MIN_BRIGHTNESS = 10.0f;
        const float MAX_BRIGHTNESS = 50.0f;

        colorsChanged |= ImGui::SliderFloat("Brightness 1", &brightness[0], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
        colorsChanged |= ImGui::SliderFloat("Brightness 2", &brightness[1], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
        colorsChanged |= ImGui::SliderFloat("Brightness 3", &brightness[2], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
        colorsChanged |= ImGui::SliderFloat("Brightness 4", &brightness[3], MIN_BRIGHTNESS, MAX_BRIGHTNESS);
        colorsChanged |= ImGui::SliderFloat("Brightness 5", &brightness[4], MIN_BRIGHTNESS, MAX_BRIGHTNESS);

        if (colorsChanged) {
            updateLampColors();
        }

        ImGui::Spacing();
        ImGui::Text("Final lamp colors (with brightness applied):");
        for (int i = 0; i < 5; i++) {
            ImGui::ColorButton(("Final " + std::to_string(i + 1)).c_str(),
                               ImVec4(lampColors[i].r, lampColors[i].g, lampColors[i].b, 1.0f),
                               ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_HDR, ImVec2(50, 20));
            ImGui::SameLine();
            ImGui::Text("Lamp %d: (%.1f, %.1f, %.1f)", i + 1, lampColors[i].r, lampColors[i].g, lampColors[i].b);
        }

        ImGui::End();

        graphics->end_gui();
    }

} // namespace app
