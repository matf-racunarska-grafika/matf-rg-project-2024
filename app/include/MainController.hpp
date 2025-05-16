//
// Created by ana on 5/13/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void draw_car();

    void draw_traffic_light();

    void update_camera();

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

public:
    std::string_view name() const override { return "app::MainController"; }
};

}// app

#endif //MAINCONTROLLER_HPP
