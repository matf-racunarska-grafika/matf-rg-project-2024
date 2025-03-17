#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/PostProcessingController.hpp>

#include <GuiController.hpp>
#include <ProgramState.hpp>
#include <imgui.h>

namespace app {
    void GuiController::initialize() {
    }

    void GuiController::draw() {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->begin_gui();

        ImGui::SetNextWindowSize(ImVec2(300, 250));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

        ImGui::Begin("Game Menu", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.15f);
        if (ImGui::Button(" PLAY GAME ", ImVec2(ImGui::GetWindowWidth() * 0.7f, 60))) {
            set_enable(false);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        unsigned int diff = Settings::getInstance().difficulty;
        float skullSpeed  = Settings::getInstance().skullSpeed;
        ImGui::Text("- Select Your Doom -");
        if (ImGui::RadioButton("EASY", diff == 0)) {
            diff       = 0;
            skullSpeed = 100.0f;
        }
        if (ImGui::RadioButton("NORMAL", diff == 1)) {
            diff       = 1;
            skullSpeed = 160.0f;
        }
        if (ImGui::RadioButton("HARD", diff == 2)) {
            diff       = 2;
            skullSpeed = 240.0f;
        }
        if (ImGui::RadioButton("IMPOSSIBLE", diff == 3)) {
            diff       = 3;
            skullSpeed = 550.0f;
        }
        Settings::getInstance().skullSpeed = skullSpeed;
        Settings::getInstance().difficulty = diff;

        if (ImGui::CollapsingHeader("Light Settings")) {
            ImGui::Text("Adjust Light Color:");
            ImGui::SliderFloat("Red", &Settings::getInstance().lightColor.x, 0.0f, 12.0f);
            ImGui::SliderFloat("Green", &Settings::getInstance().lightColor.y, 0.0f, 12.0f);
            ImGui::SliderFloat("Blue", &Settings::getInstance().lightColor.z, 0.0f, 12.0f);
        }

        auto ppc                        = engine::core::Controller::get<engine::graphics::PostProcessingController>();
        engine::graphics::Filter filter = ppc->get_active_filter();
        if (ImGui::CollapsingHeader("Cool Filters")) {
            ImGui::Text("Choose your filter:");
            if (ImGui::RadioButton("NONE", filter == engine::graphics::Filter::NOFILTER)) {
                ppc->set_active_filter(engine::graphics::Filter::NOFILTER);
            }
            if (ImGui::RadioButton("VOID", filter == engine::graphics::Filter::NEGATIVE)) {
                ppc->set_active_filter(engine::graphics::Filter::NEGATIVE);
            }
            if (ImGui::RadioButton("NOIR", filter == engine::graphics::Filter::GRAYSCALE)) {
                ppc->set_active_filter(engine::graphics::Filter::GRAYSCALE);
            }
            if (ImGui::RadioButton("FRIED", filter == engine::graphics::Filter::DEEPFRIED)) {
                ppc->set_active_filter(engine::graphics::Filter::DEEPFRIED);
            }
            if (ImGui::RadioButton("LINE", filter == engine::graphics::Filter::OUTLINE)) {
                ppc->set_active_filter(engine::graphics::Filter::OUTLINE);
            }
            if (ImGui::RadioButton("B&W", filter == engine::graphics::Filter::BLACKWHITE)) {
                ppc->set_active_filter(engine::graphics::Filter::BLACKWHITE);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Spectator mode ignores game \nelements and allows free movement.\n(exit on F)");
        if (ImGui::Button("SPECTATOR MODE", ImVec2(ImGui::GetWindowWidth() * 0.5f, 30))) {
            set_enable(false);
            Settings::getInstance().spectatorMode = true;
        }

        ImGui::End();
        ImGui::PopStyleColor(5);

        graphics->end_gui();
    }
} // app
