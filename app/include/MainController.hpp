//
// Created by matfrg on 7/14/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void begin_draw() override;

    void draw_floor();

    void draw_lamp();

    void draw_lantern();

    void draw_dog();

    void draw_graves();

    void draw_car();

    void update() override;

    void update_camera();

    void end_draw() override;

    void draw() override;

public:
    std::string_view name() const override { return "app::MainController"; }
};

}// app

#endif //MAINCONTROLLER_HPP
