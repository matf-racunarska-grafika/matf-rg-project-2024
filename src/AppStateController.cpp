//
// Created by spaske on 12/2/24.
//
#include <AppStateController.hpp>
#include <engine/controller/ControllerManager.hpp>
#include <engine/platform/Platform.hpp>

std::unique_ptr<AppStateController> AppStateController::create() {
    return std::make_unique<AppStateController>();
}

void AppStateController::initialize() {
    rg::trace();
     m_platform_controller = rg::ControllerManager::get<rg::PlatformController>();
    m_projection          = glm::perspective(glm::radians(m_camera.Zoom),
                                    static_cast<float>(m_platform_controller->window()->width()) /
                                    m_platform_controller->window()->height(),
                                    0.1f, 100.0f);
}

void AppStateController::terminate() {
    rg::trace();
}


bool AppStateController::loop() {
    return true;
}

void AppStateController::update() {
    m_projection = glm::perspective(glm::radians(m_camera.Zoom),
                                    static_cast<float>(m_platform_controller->window()->width()) /
                                    m_platform_controller->window()->height(),
                                    0.1f, 100.0f);
    update_camera();
}

void AppStateController::poll_events() {
}

void AppStateController::draw() {
}

void AppStateController::update_camera() {
    float dt = m_platform_controller->dt();
    if (m_platform_controller->key(rg::KEY_W).state() == rg::Key::State::Pressed) {
        m_camera.process_keyboard(rg::FORWARD, dt);
    }
    if (m_platform_controller->key(rg::KEY_S).state() == rg::Key::State::Pressed) {
        m_camera.process_keyboard(rg::BACKWARD, dt);
    }
    if (m_platform_controller->key(rg::KEY_A).state() == rg::Key::State::Pressed) {
        m_camera.process_keyboard(rg::LEFT, dt);
    }
    if (m_platform_controller->key(rg::KEY_D).state() == rg::Key::State::Pressed) {
        m_camera.process_keyboard(rg::RIGHT, dt);
    }
}
