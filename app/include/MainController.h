//
// Created by aleksa on 1.6.25..
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

    void draw_skybox();

    bool loop() override;

    void draw() override;

    void begin_draw() override;

    void end_draw() override;

    void draw_basketball();

    void update() override;

    void update_camera();

public:
    std::string_view name() const override { return "App::MainController"; }
};

}// app

#endif //MAINCONTROLLER_H
