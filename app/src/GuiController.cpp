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

    auto goal = engine::core::Controller::get<engine::resources::ResourcesController>()->model("ball");
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
            ImGui::Text("Worldup : (%f, %f, %f)", c.WorldUp.x, c.WorldUp.y, c.WorldUp.z);
            ImGui::EndTabItem();
        }

        // if (ImGui::BeginTabItem("teren")) {
        //     // ImGui::Text("Loaded from: %s", goal->path().c_str());
        //     ImGui::DragFloat("Teren scale", &mainController->trava_skalirano, 0.005, 0.0, 10.0);
        //     ImGui::DragFloat("Teren x", &mainController->trava_x, 0.005, -100.0, 100.0);
        //     ImGui::DragFloat("Teren y", &mainController->trava_y, 0.005, -100.0, 100.0);
        //     ImGui::DragFloat("Teren z", &mainController->trava_z, 0.005, -100.0, 100.0);
        //     ImGui::DragFloat("Ugao trava", &mainController->ugao_trava, 0.50, -100.0, 100.0);
        //     ImGui::DragFloat("Vektor x", &mainController->trava_xv, 0.005, -1.0f, 1.0f);
        //     ImGui::DragFloat("Vektor y", &mainController->trava_yv, 0.005, -1.0f, 1.0f);
        //     ImGui::DragFloat("Vektor z", &mainController->trava_zv, 0.005, -1.0f, 1.0f);
        //     ImGui::EndTabItem();
        // }

    }

    ImGui::End();
    graphics->end_gui();
}
}
