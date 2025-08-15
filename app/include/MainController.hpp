//
// Created by strahinjas on 3/18/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/core/Engine.hpp>

namespace my_project {
class MyController final : public engine::core::Controller {
public:
    [[nodiscard]] std::string_view name() const override { return "my_project::MyController"; };

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    // Simple abstraction functions
    //void draw_test_model();

    void draw_light_cube();

    void draw_island_model();

    void draw_light_source_birds();

    void draw_model_dog();

    void draw_skybox();

    // Setting up the initial camera parameters
    void initialize_camera();

    void set_timer(float delay_seconds);

    void scene_event1();

    class Timer {
    public:
        explicit Timer(float delay_seconds) : m_delay_time(delay_seconds)
                                          , m_start_time(std::chrono::steady_clock::now()) {}

        bool has_expired() const {
            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration_cast<std::chrono::duration<float> >(now - m_start_time).count();
            return elapsed >= m_delay_time;
        }

    private:
        float m_delay_time;
        std::chrono::steady_clock::time_point m_start_time;
    };

    bool m_event1_in_waiting{false};
    bool m_draw_dog{true};
    static Timer inner_event_timer;
    glm::vec3 m_directional_strength = {0.8f, 0.8f, 0.8f};
    bool m_enable_gui{false};
    bool m_enable_cursor{true};
};
}

#endif //MAINCONTROLLER_HPP
