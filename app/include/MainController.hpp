//
// Created by filipn on 9/1/25.
//

#ifndef MAINCONTROLER_HPP
#define MAINCONTROLER_HPP
#include <engine/core/Controller.hpp>

namespace app {
class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void draw_moon();

    void draw_sun();

    void draw_island();

    void update_camera();

    void update() override;

    void begin_draw() override;

    void draw_skybox();

    void draw_tree();

    void draw_bench();

    void draw() override;

    void end_draw() override;

public:
    std::string_view name() const override { return "MainController"; }
};
}
#endif //MAINCONTROLER_HPP
