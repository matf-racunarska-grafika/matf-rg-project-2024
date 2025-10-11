#include <imgui.h>
#include <engine/core/Engine.hpp>
#include <GUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace app {
void GUIController::initialize() {
    set_enable(false);
}

void GUIController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_F2)
                .state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
    }

    float dt = platform->dt();
    if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) {
        pointLightIntensity += 0.5f * dt;
    }
    if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) {
        pointLightIntensity -= 0.5f * dt;
        if (pointLightIntensity < 0.0f) {
            pointLightIntensity = 0.0f;
        }
    }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    graphics->begin_gui();

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
    graphics->end_gui();
}
}
