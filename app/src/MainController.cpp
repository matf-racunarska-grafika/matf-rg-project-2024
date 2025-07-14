//
// Created by cvnpko on 7/14/25.
//

#include "../include/MainController.hpp"

namespace app {
void MainController::initialize() { engine::graphics::OpenGL::enable_depth_testing(); }

bool MainController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) { return false; }
    return true;
}

void MainController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    }
}

void MainController::update() {}

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw() {}

void MainController::end_draw() { engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers(); }

}// app