//
// Created by cvnpko on 7/15/25.
//

#ifndef EVENTCONTROLLER_HPP
#define EVENTCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

namespace app {
class MainEventController final : public engine::core::Controller {
public:
    std::string_view name() const override { return "app::EventController"; }

    bool is_day() const { return m_is_day; };

    float get_event_angle() const { return m_event_degree; }

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    bool m_is_day{true};
    int m_num_of_days{0};
    bool m_spinning{true};
    const float m_skybox_changing_t{15.0f};
    float m_skybox_changed_time{0.0f};
    float m_event_degree{0.0f};
    float m_time_scale{60.0f};
};
}

#endif //EVENTCONTROLLER_HPP
