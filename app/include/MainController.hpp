//
// Created by teodora on 4.7.25..
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include<engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;
    bool loop() override;
    void draw_floor();
    void draw_bench();
    void update_camera();
    void update() override;
    void begin_draw() override;
    void draw_streetlamp();
    void setup_lighting();
    void draw() override;
    void end_draw() override;

public:
    std::string_view name() const override {
        return "app:MainController";
    }
};

} // app

#endif //MAINCONTROLLER_HPP
