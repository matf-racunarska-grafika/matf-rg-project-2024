#include "GUIController.hpp"
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <imgui.h>
#include "MainController.h"

namespace app {
void GUIController::initialize() {
    set_enable(true);
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

    ImGui::Begin("Camera info");
    const auto &c = *camera;
    ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
    ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
    ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
    ImGui::End();

    if (auto mainCtrl = engine::core::Controller::get<app::MainController>()) {
        auto &dir = mainCtrl->dir_light();
        auto &pt  = mainCtrl->point_light();
        auto &sh  = mainCtrl->shininess();
        ImGui::Begin("Lighting");
        ImGui::Text("Directional Light");
        ImGui::DragFloat3("dir.direction", &dir.direction.x, 0.01f);
        ImGui::ColorEdit3("dir.ambient", &dir.ambient.x);
        ImGui::ColorEdit3("dir.diffuse", &dir.diffuse.x);
        ImGui::ColorEdit3("dir.specular", &dir.specular.x);
        ImGui::Separator();
        ImGui::Text("Point Light");
        ImGui::DragFloat3("pt.position", &pt.position.x, 0.1f);
        ImGui::DragFloat("pt.constant", &pt.constant, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("pt.linear", &pt.linear, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("pt.quadratic", &pt.quadratic, 0.001f, 0.0f, 1.0f);
        ImGui::ColorEdit3("pt.ambient", &pt.ambient.x);
        ImGui::ColorEdit3("pt.diffuse", &pt.diffuse.x);
        ImGui::ColorEdit3("pt.specular", &pt.specular.x);
        ImGui::Separator();
        ImGui::DragFloat("shininess", &sh, 1.0f, 1.0f, 256.0f);
        ImGui::End();
    }

    graphics->end_gui();
}
}


