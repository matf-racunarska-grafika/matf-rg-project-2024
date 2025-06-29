#include <imgui.h>
#include <engine/core/Engine.hpp>
#include <app/GUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <GLFW/glfw3.h>
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

    ImGui::SliderFloat("Point Light Intensity", &mainCtrl->pointLightIntensity, 0.0f, 5.0f);

    if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
        // Odmah primeni promenu
        auto *platform = core::Controller::get<platform::PlatformController>();
        GLFWwindow *win = platform->window()->handle_();

        if (fullscreen) {
            // Udji u fullscreen
            GLFWmonitor *mon = glfwGetPrimaryMonitor();
            const auto *mode = glfwGetVideoMode(mon);
            glfwSetWindowMonitor(win, mon,
                                 0, 0,
                                 mode->width, mode->height,
                                 mode->refreshRate);
        } else {
            // Vracanje u windowed rezim
            int w = cfg.value("width", 800);
            int h = cfg.value("height", 600);
            glfwSetWindowMonitor(win, nullptr,
                                 100, 100,
                                 w, h,
                                 0);
        }

        // Cuvamo promenu u configu
        cfg["fullscreen"] = fullscreen;
    }

    ImGui::End();
    graphics->end_gui();
}
}
