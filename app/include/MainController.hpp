#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <spdlog/spdlog.h>

namespace app {
class MainController : public engine::core::Controller {

    void initialize() override;
    bool loop() override;

    void begin_draw() override;
    void end_draw() override;
    void draw() override;

    void draw_backpack();
    void draw_skybox();
    void draw_island();
    void draw_tree1();
    void draw_tree2();
    void draw_vegetation();
    void draw_mushroom();
    void draw_campfire();
    void draw_logs();
    void draw_lighthouse();

    void update() override;

    void update_camera();


public:
    std::string_view name() const override {
        return "app::MainController";
    }
};
}

#endif //MAINCONTROLLER_HPP
