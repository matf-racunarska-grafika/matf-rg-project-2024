//
// Created by filip on 1/22/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;
    bool loop() override;
    void draw_backpack();
    void update_camera();
    void update() override;
    void begin_draw() override;
    void draw_skyboxes();

    void draw_ground();

    void draw_house();

    void draw_tree1();

    void draw_tree2();

    void draw_sun();

    void draw_lamp();

    void draw() override;
    void end_draw() override;
    void draw_ufo();

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

}



#endif //MAINCONTROLLER_HPP
