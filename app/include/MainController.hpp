//
// Created by ognje on 7/14/2025.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {

    bool m_headlightsOn = true;
    bool m_prevT = false;
    float m_chairPhase = 0.0f;

    bool m_showChair = false;
    float m_headlightsOffTimer = 0.0f;

    void initialize() override;

    bool loop() override;

    void draw_backpack();

    void draw_army_truck();

    void draw_house1();

    void draw_house2();

    void draw_fence();

    void draw_floor();

    void draw_lamp();

    void draw_chair();

    void draw_rocking_chair();

    void update_camera();

    void update() override;

    void begin_draw() override;

    void draw_skybox();

    void draw() override;

    void end_draw() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

};// app

#endif //MAINCONTROLLER_HPP
