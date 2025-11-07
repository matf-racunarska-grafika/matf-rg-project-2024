#include <MyGUIController.hpp>
#include <MySceneController.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <imgui.h>

namespace app {
void MyGUIController::initialize() {
    set_enable(false);
}

void MyGUIController::poll_events() {
    const engine::platform::PlatformController *platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_F2)
                .state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
                }
}

void MyGUIController::draw() {
    engine::graphics::GraphicsController *graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::graphics::Camera *camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    graphics->begin_gui();


    // Draw camera info
    ImGui::Begin("Camera info");
    const auto &c = *camera;
    ImGui::Text("Camera position: (%f, %f, %f)", c.Position
                                                  .x, c.Position
                                                       .y, c.Position
                                                            .z);
    ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
    ImGui::Text("Camera front: (%f, %f, %f)", c.Front
                                               .x, c.Front
                                                    .y, c.Front
                                                         .z);
    ImGui::End();

    ImGui::Begin("Lighting");
    static float brightness = 1.0f;
    if (ImGui::SliderFloat("Intensity of light sources:", &brightness, 0.0f, 2.0f)) {
          engine::core::Controller::get<app::MySceneController>()->set_dim(brightness);
    }

    {
        auto scene = engine::core::Controller::get<app::MySceneController>();
        bool enabled = scene->scene()->bloom_enabled();
        if (ImGui::Checkbox("Bloom", &enabled)) {
            scene->scene()->set_bloom_enabled(enabled);
        }
        ImGui::SameLine();
        ImGui::Text("Toggle bloom");
    }

    static float treshold = 1.0f;
    if (ImGui::SliderFloat("Bloom threshold:", &treshold, 0.0f, 2.0f)) {
        engine::core::Controller::get<app::MySceneController>()->set_threshold(treshold);
    }
    static float intensity = 1.0f;
    if (ImGui::SliderFloat("Bloom intensity:", &intensity, 0.0f, 2.0f)) {
        engine::core::Controller::get<app::MySceneController>()->set_bloom_intensity(intensity);
    }

    {
        auto scene = engine::core::Controller::get<app::MySceneController>();
        bool enabled = scene->scene()->swarm_enabled();
        if (ImGui::Checkbox("Swarm", &enabled)) {
            scene->scene()->set_swarm_enabled(enabled);
        }
        ImGui::SameLine();
        ImGui::Text("Toggle light swarm");
    }

    ImGui::End();
    graphics->end_gui();
}
}
