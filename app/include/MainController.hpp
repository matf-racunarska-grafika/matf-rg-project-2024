//
// Created by boce on 7/26/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace app {

class MainController final : public engine::core::Controller {
    void initialize() override;
    bool loop() override;

    void begin_draw() override;
    void draw() override;
    void end_draw() override;

    void terminate() override;

    void create_plane();
    void draw_plane();
    void destroy_plane();

    unsigned vbo_plane{0};
    unsigned vao_plane{0};
};

}// app

#endif //MAINCONTROLLER_HPP
