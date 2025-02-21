//
// Created by viktor on 21.2.25..
//
#include <engine/graphics/GraphicsController.hpp>
#include <imgui.h>
#include <GuiController.hpp>
#include <ProgramState.hpp>

namespace app {
    void GuiController::initialize() {
    }

    void GuiController::draw() {
        bool imguiActive = false;
        bool game = false;
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

        unsigned int diff = Settings::getInstance().difficulty;
        float skullSpeed = Settings::getInstance().skullSpeed;
        ImGui::Text("- Select Your Doom -");
        if (ImGui::RadioButton("EASY", diff == 0)) { diff = 0; skullSpeed = 100.0f; }
        if (ImGui::RadioButton("NORMAL", diff == 1)) { diff = 1; skullSpeed = 160.0f; }
        if (ImGui::RadioButton("HARD", diff == 2)) { diff = 2; skullSpeed = 240.0f; }
        if (ImGui::RadioButton("IMPOSSIBLE", diff == 3)) { diff = 3; skullSpeed = 550.0f; }
        Settings::getInstance().skullSpeed = skullSpeed;
        Settings::getInstance().difficulty = diff;

        if (ImGui::CollapsingHeader("Light Settings")) {
            ImGui::Text("Adjust Light Color:");
            ImGui::SliderFloat("Red", &Settings::getInstance().lightColor.x, 0.0f, 10.0f);
            ImGui::SliderFloat("Green", &Settings::getInstance().lightColor.y, 0.0f, 10.0f);
            ImGui::SliderFloat("Blue", &Settings::getInstance().lightColor.z, 0.0f, 10.0f);
        }

        ImGui::End();
        ImGui::PopStyleColor(5);

        graphics->end_gui();
    }
} // app