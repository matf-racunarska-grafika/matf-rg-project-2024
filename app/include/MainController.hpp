#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <spdlog/spdlog.h>
#include <cstdlib>

namespace app {

    extern float test_x, test_y, test_z;
    extern float test_rotation;
    extern float test_scale;


class MainController : public engine::core::Controller {

    void initialize() override;
    bool loop() override;
    void begin_draw() override;
    void end_draw() override;
    void draw() override;

    void draw_backpack();
    void test();

    void draw_skybox();
    void draw_island();

    void draw_campfire();
    void draw_terrain();

    void drawLightSource_day();
    void drawLightSource_night();

    void draw_water();

    void update() override;
    void update_camera();

public:

    std::string_view name() const override {
        return "app::MainController";
    }
};
}

#endif //MAINCONTROLLER_HPP
