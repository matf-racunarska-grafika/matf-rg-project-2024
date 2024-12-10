//
// Created by spaske on 12/9/24.
//
#include <imgui.h>
#include <engine/Engine.hpp>
#include <app/GUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace engine::test::app {
    void GUIController::initialize() {
        set_enable(false);
    }

    void GUIController::poll_events() {
        const auto platform = engine::controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_F2).state() == engine::platform::Key::State::JustPressed) {
            set_enable(!is_enabled());
        }
    }

    void GUIController::draw() {
        auto graphics = engine::controller::get<engine::graphics::GraphicsController>();
        auto camera   = engine::controller::get<engine::graphics::GraphicsController>()->camera();
        graphics->begin_gui();
        // Draw backpack scale slider window
        // auto backpack  = engine::controller::get<engine::resources::ResourcesController>()->model("backpack");
        // static float f = 0.0f;
        // ImGui::Begin(backpack->name().c_str());
        // ImGui::Text("Loaded from: %s", backpack->path().c_str());
        // ImGui::DragFloat("Backpack scale", &m_backpack_scale, 0.05, 0.1, 4.0);
        // ImGui::End();

        // Draw camera info
        ImGui::Begin("Camera info");
        const auto &c = *camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::End();
        graphics->end_gui();
    }
}