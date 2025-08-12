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

    auto goal = engine::core::Controller::get<engine::resources::ResourcesController>()->model("goal");
    static float f = 0.0f;
    ImGui::Begin("INFO");


    // Draw camera info

    if (ImGui::BeginTabBar("TABOVI")) {
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
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("GOAL")) {
            ImGui::Text("Loaded from: %s", goal->path().c_str());
            ImGui::DragFloat("football_goal scale", &mainController->goal_scale, 0.035, 0.0, 5.0);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
    graphics->end_gui();
}
}
