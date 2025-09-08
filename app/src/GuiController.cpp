//
// Created by aleksa on 12.8.25..
//

#include <imgui.h>
#include <engine/core/Engine.hpp>
#include <GuiController.hpp>
#include <MainController.h>
#include <engine/graphics/GraphicsController.hpp>

namespace app {
void GUIController::initialize() { set_enable(false); }

void GUIController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_K)
                .state() == engine::platform::Key::State::JustPressed) { set_enable(!is_enabled()); }
}

void GUIController::draw() {
    auto mainController = engine::core::Controller::get<app::MainController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    graphics->begin_gui();
    const auto &c = *camera;

    static float f = 0.0f;
    ImGui::Begin("INFO");


    if (ImGui::BeginTabBar("TABS")) {
        if (ImGui::BeginTabItem("Camera")) {
            ImGui::Text("Camera position: (%f, %f, %f)", c.Position
                                                          .x, c.Position
                                                               .y, c.Position
                                                                    .z);
            ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
            ImGui::Text("Camera front: (%f, %f, %f)", c.Front
                                                       .x, c.Front
                                                            .y, c.Front
                                                                 .z);
            ImGui::Text("Worldup : (%f, %f, %f)", c.WorldUp.x, c.WorldUp.y, c.WorldUp.z);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Directional light")) {
            ImGui::DragFloat3("Direction", &mainController->dirLightDirection.x, 0.01, 0.0, 1.0);

            ImGui::DragFloat3("Ambient color", &mainController->dirLightColorAmbient.x, 0.01, 0.0, 100.0);
            ImGui::DragFloat3("Diffuse color", &mainController->dirLightColorDiffuse.x, 0.01, 0.0, 100.0);
            ImGui::DragFloat3("Specular color", &mainController->dirLightColorSpecular.x, 0.01, 0.0, 100.0);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Point Light")) {
            ImGui::DragFloat3("Position", &mainController->sun_pos.x, 0.1);
            ImGui::DragFloat3("Ambient color", &mainController->pointLightColorAmbient.x, 0.01, 0.0, 100.0);
            ImGui::DragFloat3("Diffuse color", &mainController->pointLightColorDiffuse.x, 0.01, 0.0, 100.0);
            ImGui::DragFloat3("Specular color", &mainController->pointLightColorSpecular.x, 0.01, 0.0, 100.0);
            ImGui::DragFloat3("Emission color", &mainController->emissive_color.x, 0.1, 0.0, 100.0);
            ImGui::DragFloat("Emission strength", &mainController->emission_strength, 0.01, 0.0, 20.0);
            ImGui::DragFloat("Constant", &mainController->pointLightConstant, 0.001, 0.0, 1.0);
            ImGui::DragFloat("Linear", &mainController->pointLightLinear, 0.001, 0.0, 1.0, "%.5f");
            ImGui::DragFloat("Quadratic", &mainController->pointLightQuadratic, 0.000001f, 0.00000f, 1.00000, "%.6f");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }


    ImGui::End();
    graphics->end_gui();
}
}
