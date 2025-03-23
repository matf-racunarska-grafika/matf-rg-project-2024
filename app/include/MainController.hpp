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

    class Timer {
    public:
        explicit Timer(float delay_seconds) : delay_time(delay_seconds)
                                          , start_time(std::chrono::steady_clock::now()) {}

        bool has_expired() const {
            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration_cast<std::chrono::duration<float> >(now - start_time).count();
            return elapsed >= delay_time;
        }

    private:
        float delay_time;
        std::chrono::steady_clock::time_point start_time;
    };

    bool enable_gui{false};
    bool enable_cursor{true};
};
}

#endif //MAINCONTROLLER_HPP
