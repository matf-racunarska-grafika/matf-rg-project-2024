#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <spdlog/spdlog.h>
#include <cstdlib>

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
    void draw_tree2(const glm::vec3& position);
    void draw_forest();
    void draw_forest_instanced();
    void draw_vegetation();
    void draw_mushroom();

    void draw_campfire();
    void draw_lighthouse();

    void draw_log();
    void draw_logs();

    void draw_tents();

    void drawLightSource_day();
    void drawLightSource_night();

    void update() override;
    void update_camera();

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};
}

#endif //MAINCONTROLLER_HPP
