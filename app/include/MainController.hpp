//
// Created by cvnpko on 7/14/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <memory>
#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/core/Engine.hpp>

namespace app {

class MainController final : public engine::core::Controller {
public:
    std::string_view name() const override { return "app::MainController"; }

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw_skybox();

    void draw_dunes();

    void draw_pyramids();

    void draw_sphinx();

    void draw_camels();

    void draw() override;

    void end_draw() override;

    void update_camera();

    bool m_cursor_enabled{true};

};

}// app

#endif //MAINCONTROLLER_HPP
