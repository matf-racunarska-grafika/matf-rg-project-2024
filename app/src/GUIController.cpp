#include <imgui.h>
#include <engine/core/Engine.hpp>
#include <app/GUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <app/MainController.hpp>

namespace engine::myapp {
void GUIController::initialize() { set_enable(false); }

void GUIController::poll_events() {
    const auto platform = engine::core::Controller::get<platform::PlatformController>();
    if (platform->key(platform::KeyId::KEY_F2)
                .state() == platform::Key::State::JustPressed) { set_enable(!is_enabled()); }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    auto mainCtrl = engine::core::Controller::get<MainController>();

    graphics->begin_gui();
    auto &cfg = engine::util::Configuration::config()["window"];
    static bool fullscreen = cfg.value("fullscreen", false);

    ImGui::Begin("Settings");

    ImGui::SliderFloat("Point Light Intensity", &mainCtrl->g_point_light_intensity, 0.0f, 7.0f);
    ImGui::SliderFloat3("Light Position", &mainCtrl->g_light_pos.x,
                        -200.0f, 200.0f,
                        "%.1f");

    if (ImGui::Checkbox("Fullscreen", &fullscreen)) { core::Controller::get<platform::PlatformController>()->set_fullscreen(fullscreen); }

    // MSAA
    ImGui::Checkbox("Enable MSAA", &mainCtrl->g_msaa_enabled);

    ImGui::End();
    graphics->end_gui();
}
}
