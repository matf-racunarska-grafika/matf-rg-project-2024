//
// Created by matfrg on 11/1/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <MyApp.hpp>
#include <engine/core/Controller.hpp>

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

    bool enable_cursor{true};

    void draw_test_model();

    void draw_light_cube();

};
}

#endif //MAINCONTROLLER_HPP
