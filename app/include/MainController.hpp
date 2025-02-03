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

    void update();

    void update_camera();


public:
    std::string_view name() const override {
        return "app::MainController";
    }
};
}

#endif //MAINCONTROLLER_HPP
