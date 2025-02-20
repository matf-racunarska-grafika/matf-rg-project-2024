//
// Created by viktor on 21.2.25..
//
#include <engine/graphics/GraphicsController.hpp>
#include <imgui.h>
#include <GuiController.hpp>

namespace app {
    void GuiController::initialize() {
    }

    void GuiController::draw() {
        int difficulty = 1;
        bool imguiActive = false;
        bool game = false;
        float skullSpeed = 100;
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->begin_gui();

        ImGui::SetNextWindowSize(ImVec2(300, 250));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

        ImGui::Begin("Game Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.15f);
        if (ImGui::Button(" PLAY GAME ", ImVec2(ImGui::GetWindowWidth() * 0.7f, 60))) {
            game = !game;
            imguiActive = false;
            set_enable(false);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("- Select Your Doom -");
        if (ImGui::RadioButton("EASY", difficulty == 0)) { difficulty = 0; skullSpeed = 100; }
        if (ImGui::RadioButton("NORMAL", difficulty == 1)) { difficulty = 1; skullSpeed = 160; }
        if (ImGui::RadioButton("HARD", difficulty == 2)) { difficulty = 2; skullSpeed = 240; }
        if (ImGui::RadioButton("IMPOSSIBLE", difficulty == 3)) { difficulty = 3; skullSpeed = 550; }

        if (ImGui::CollapsingHeader("Light Settings")) {
            ImGui::Text("Adjust Light Color:");
            ImGui::SliderFloat("Red", &lightColor.x, 0.0f, 10.0f);
            ImGui::SliderFloat("Green", &lightColor.y, 0.0f, 10.0f);
            ImGui::SliderFloat("Blue", &lightColor.z, 0.0f, 10.0f);
        }

        ImGui::End();
        ImGui::PopStyleColor(5);

        graphics->end_gui();
    }
} // app