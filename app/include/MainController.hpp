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
    void initalize_camera();

    bool enable_cursor{true};
};
}

#endif //MAINCONTROLLER_HPP
