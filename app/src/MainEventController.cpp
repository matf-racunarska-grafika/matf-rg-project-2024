//
// Created by cvnpko on 7/15/25.
//

#include <MainEventController.hpp>
#include <engine/core/Engine.hpp>
#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void MainEventController::initialize() { set_enable(false); }

bool MainEventController::loop() { return true; }

void MainEventController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_F2).state() == engine::platform::Key::State::JustPressed) {
        set_enable(true);
        spdlog::info(m_spinning);
        m_num_of_days++;
    }
}

void MainEventController::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (m_spinning) {
        if (m_is_day && m_event_degree + m_time_scale * platform->dt() > 180.0f) {
            m_event_degree = 180.0f;
            m_spinning = false;
        } else if (!m_is_day && m_event_degree + m_time_scale * platform->dt() > 360.0f) {
            m_event_degree = 0.0f;
            m_spinning = false;
        } else { m_event_degree += m_time_scale * platform->dt(); }
    } else {
        if (m_skybox_changed_time + m_time_scale * platform->dt() > m_skybox_changing_t) {
            m_skybox_changed_time = 0.0f;
            m_is_day = !m_is_day;
            m_spinning = true;
            if (--m_num_of_days == 0) { set_enable(false); }
        } else { m_skybox_changed_time += m_time_scale * platform->dt(); }
    }
}

void MainEventController::begin_draw() {}

void MainEventController::draw() {}

void MainEventController::end_draw() {}
}