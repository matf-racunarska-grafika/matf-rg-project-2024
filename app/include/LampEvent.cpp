//
// Created by matfrg on 7/16/25.
//

#include "LampEvent.hpp"

#include <MainController.hpp>
#include <engine/core/Controller.hpp>
#include <spdlog/spdlog.h>

namespace app {
#include "LampEvent.hpp"
#include "MainController.hpp"
#include <glm/glm.hpp>

LampEvent::LampEvent() {
    lamp_state = TRANSPARENT;
    t_timer = 0.0f;
    cycle_active = false;
    cycle_stage = 0;
}

void LampEvent::start_automatic_cycle() {
    lamp_state = TRANSPARENT;
    t_timer = 0.0f;
    cycle_active = true;
    cycle_stage = 0;
}

void LampEvent::update_lamp(float delta_time) {
    auto main_controller = engine::core::Controller::get<MainController>();

    if (cycle_active) {
        t_timer += delta_time;

        switch (cycle_stage) {
            case 0: // From TRANSPARENT to GREEN after 2s
                if (t_timer >= 2.0f) {
                    lamp_state = GREEN;
                    t_timer = 0.0f;
                    cycle_stage = 1;
                }
                break;
            case 1: // From GREEN to BLUE after 2s
                if (t_timer >= 2.0f) {
                    lamp_state = BLUE;
                    t_timer = 0.0f;
                    cycle_stage = 2;
                }
                break;
            case 2: // From BLUE to TRANSPARENT after 3s
                if (t_timer >= 3.0f) {
                    lamp_state = TRANSPARENT;
                    t_timer = 0.0f;
                    cycle_stage = 3;
                    cycle_active = false;
                }
                break;
        }
    }

    if (lamp_state == TRANSPARENT) { main_controller->point_light.intensity = glm::vec3(1.0f); } else if (lamp_state == GREEN) { main_controller->point_light.intensity = glm::vec3(0.1f, 5.0f, 0.0f); } else if (lamp_state == BLUE) { main_controller->point_light.intensity = glm::vec3(0.0f, 0.0f, 5.0f); }
}

LampState LampEvent::get_lamp_state() const { return lamp_state; }
}// app