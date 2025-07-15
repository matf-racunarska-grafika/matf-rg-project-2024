#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {
class MainController  : public engine::core::Controller
{
    void initialize() override;

    bool loop() override;
    void draw_temple();
    void update() override;
    void update_camera();
    void begin_draw() override;
    void draw_petal();
    void draw() override;
    void draw_skybox();
    void draw_lamp();
    void end_draw() override;



public:
    std::string_view name() const override { return "app::MainController"; }

};
}



#endif //MAINCONTROLLER_HPP