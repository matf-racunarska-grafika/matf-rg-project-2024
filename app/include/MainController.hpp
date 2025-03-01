#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <spdlog/spdlog.h>
#include <cstdlib>

namespace app {

    extern float test_x, test_y, test_z;
    extern float test_rotation_x, test_rotation_y, test_rotation_z;
    extern float test_scale;
    extern float wind_speed;


class MainController : public engine::core::Controller {

    void initialize() override;
    bool loop() override;
    void begin_draw() override;
    void end_draw() override;
    void draw() override;

    void test();

    void draw_skybox();
    void draw_terrain();
    void draw_water();
    void draw_forest();

    void draw_campfire();
    void draw_logs();
    void draw_tents();
    void draw_old_tree();
    void draw_bushes();
    void draw_white_flowers();
    void draw_red_flowers();
    void draw_flowers();
    void draw_path();
    void draw_mushrooms();
    void draw_lighthouse();
    void draw_stones();
    void draw_fire();

    void update() override;
    void update_camera();

public:

    std::string_view name() const override {
        return "app::MainController";
    }
};
}

#endif //MAINCONTROLLER_HPP
