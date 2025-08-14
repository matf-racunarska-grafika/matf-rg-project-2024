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

        if (ImGui::BeginTabItem("teren")) {
            // ImGui::Text("Loaded from: %s", goal->path().c_str());
            ImGui::DragFloat("Teren scale", &mainController->trava_skalirano, 0.005, 0.0, 10.0);
            ImGui::DragFloat("Teren x", &mainController->trava_x, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Teren y", &mainController->trava_y, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Teren z", &mainController->trava_z, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Ugao trava", &mainController->ugao_trava, 0.50, -100.0, 100.0);
            ImGui::DragFloat("Vektor x", &mainController->osa_x, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor y", &mainController->osa_y, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor z", &mainController->osa_z, 0.005, -1.0f, 1.0f);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("lopta")) {
            // ImGui::Text("Loaded from: %s", goal->path().c_str());
            ImGui::DragFloat("Lopta scale", &mainController->lopta_skalirano, 0.005, 0.0, 10.0);
            ImGui::DragFloat("Lopta x", &mainController->lopta_x, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Lopta y", &mainController->lopta_y, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Lopta z", &mainController->lopta_z, 0.005, -100.0, 100.0);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Gol")) {
            ImGui::Text("Loaded from: %s", goal->path().c_str());
            ImGui::DragFloat("Goal scale", &mainController->gol_skalirano, 0.005, 0.0, 10.0);
            ImGui::DragFloat("Goal x", &mainController->gol_x, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Goal y", &mainController->gol_y, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Goal z", &mainController->gol_z, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Ugao trava", &mainController->ugao_gol, 0.02, -100.0, 100.0);
            ImGui::DragFloat("Vektor x", &mainController->gosa_x, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor y", &mainController->gosa_y, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor z", &mainController->gosa_z, 0.005, -1.0f, 1.0f);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Refletkor1")) {
            ImGui::Text("Loaded from: %s", goal->path().c_str());
            ImGui::DragFloat("Goal scale", &mainController->ref_skalirano, 0.003, 0.0, 10.0);
            ImGui::DragFloat("Goal x", &mainController->ref_x, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Goal y", &mainController->ref_y, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Goal z", &mainController->ref_z, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Ugao trava", &mainController->ref1_ugao, 0.02, -100.0, 100.0);
            ImGui::DragFloat("Vektor x", &mainController->ref_xv, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor y", &mainController->ref_yv, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor z", &mainController->ref_zv, 0.005, -1.0f, 1.0f);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Refletkor2")) {
            ImGui::Text("Loaded from: %s", goal->path().c_str());
            ImGui::DragFloat("Goal scale", &mainController->ref_skalirano, 0.003, 0.0, 10.0);
            ImGui::DragFloat("Goal x", &mainController->ref2_x, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Goal y", &mainController->ref2_y, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Goal z", &mainController->ref2_z, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Ugao trava", &mainController->ref2_ugao, 1.0, -180.0, 180.0);
            ImGui::DragFloat("Vektor x", &mainController->ref2_xv, 0.005, -5.0f, 5.0f);
            ImGui::DragFloat("Vektor y", &mainController->ref2_yv, 0.005, -5.0f, 5.0f);
            ImGui::DragFloat("Vektor z", &mainController->ref2_zv, 0.005, -5.0f, 5.0f);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Crveni")) {
            // ImGui::Text("Loaded from: %s", goal->path().c_str());
            ImGui::DragFloat("Teren scale", &mainController->crveni_skalirano, 0.005, 0.0, 10.0);
            ImGui::DragFloat("Teren x", &mainController->crveni_x, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Teren y", &mainController->crveni_y, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Teren z", &mainController->crveni_z, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Ugao crveni", &mainController->ugao_crveni, 0.50, -100.0, 100.0);
            ImGui::DragFloat("Vektor x", &mainController->crveni_xv, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor y", &mainController->crveni_yv, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor z", &mainController->crveni_zv, 0.005, -1.0f, 1.0f);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("plavi")) {
            // ImGui::Text("Loaded from: %s", goal->path().c_str());
            ImGui::DragFloat("Teren scale", &mainController->plavi_skalirano, 0.005, 0.0, 10.0);
            ImGui::DragFloat("Teren x", &mainController->plavi_x, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Teren y", &mainController->plavi_y, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Teren z", &mainController->plavi_z, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Ugao plavi", &mainController->ugao_plavi, 0.50, -180.0, 180.0);
            ImGui::DragFloat("Vektor x", &mainController->plavi_xv, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor y", &mainController->plavi_yv, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor z", &mainController->plavi_zv, 0.005, -1.0f, 1.0f);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("golman")) {
            // ImGui::Text("Loaded from: %s", goal->path().c_str());
            ImGui::DragFloat("Teren scale", &mainController->golman_skalirano, 0.005, 0.0, 10.0);
            ImGui::DragFloat("Teren x", &mainController->golman_x, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Teren y", &mainController->golman_y, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Teren z", &mainController->golman_z, 0.005, -100.0, 100.0);
            ImGui::DragFloat("Ugao golman", &mainController->ugao_golman, 0.50, -180.0, 180.0);
            ImGui::DragFloat("Vektor x", &mainController->golman_xv, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor y", &mainController->golman_yv, 0.005, -1.0f, 1.0f);
            ImGui::DragFloat("Vektor z", &mainController->golman_zv, 0.005, -1.0f, 1.0f);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
    graphics->end_gui();
}
}
