#include <imgui.h>
#include <engine/core/Engine.hpp>
#include <app/GUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>



namespace engine::test::app {

bool g_lamp_is_on = true;
bool g_lamp_is_flickering = false;
bool g_start_flicker_sequence = false;
float g_flicker_timer = 0.0f;

void GUIController::initialize() {
    set_enable(false);
}

void GUIController::poll_events() {
    const auto platform = engine::core::Controller::get<platform::PlatformController>();
    if (platform->key(platform::KeyId::KEY_F2)
                .state() == platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
    }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    graphics->begin_gui();

    ImGui::Begin("Lamp Control");

    extern bool g_lamp_is_on;
    extern bool g_lamp_is_flickering;
    extern float g_flicker_timer;

    ImGui::Text("Lamp Status: %s", g_lamp_is_on ? "ON" : "OFF");

    if (g_lamp_is_flickering) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "FLICKERING...");
        ImGui::ProgressBar(g_flicker_timer / 3.0f, ImVec2(0.0f, 0.0f));
    }
    if (ImGui::Button("Toggle Lamp (O)")) {
        g_lamp_is_on = !g_lamp_is_on;
    }

    ImGui::Separator();

    if (ImGui::Button("Trigger Flicker Effect", ImVec2(200, 30))) {
        extern bool g_start_flicker_sequence;
        g_start_flicker_sequence = true;
    }

    ImGui::TextWrapped("Flicker effect: Waits 3s, flickers for 3s, then randomly turns on/off");

    ImGui::End();

    graphics->end_gui();
}
}
