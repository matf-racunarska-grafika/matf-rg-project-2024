#include <imgui.h>
#include <engine/core/Engine.hpp>
#include <GUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <MainController.hpp>
#include <iostream>

namespace app {

    void GUIController::initialize() {
        set_enable(false);
    }

    void GUIController::poll_events() {
        const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed) {
            set_enable(!is_enabled());
        }
    }

    void GUIController::draw() {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera   = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        graphics->begin_gui();
        // Draw backpack scale slider window
        // auto backpack  = engine::core::Controller::get<engine::resources::ResourcesController>()->model("backpack");
        // static float f = 0.0f;
        // ImGui::Begin(backpack->name().c_str());
        // ImGui::Text("Loaded from: %s", backpack->path().c_str());
        // ImGui::DragFloat("Backpack scale", &m_backpack_scale, 0.05, 0.1, 4.0);
        // ImGui::End();
        auto model = engine::core::Controller::get<engine::resources::ResourcesController>()->model("test");
        ImGui::Begin(model->name().c_str());
        ImGui::DragFloat("Skaliranje:", &test_scale, 0.01, 0.01, 10);
        ImGui::DragFloat("RotacijaX:", &test_rotation_x, 1, -180, 180);
        ImGui::DragFloat("RotacijaY:", &test_rotation_y, 1, -180, 180);
        ImGui::DragFloat("RotacijaZ:", &test_rotation_z, 1, -180, 180);
        ImGui::DragFloat("X:", &test_x, 1, -100, 100);
        ImGui::DragFloat("Y:", &test_y, 1, -100, 100);
        ImGui::DragFloat("Z:", &test_z, 1, 100, 100);
        ImGui::End();

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
