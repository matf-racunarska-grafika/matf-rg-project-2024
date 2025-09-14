//
// Created by boce on 7/26/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <Lights.hpp>

namespace app {

class MainPlatformEventObserver final : public engine::platform::PlatformEventObserver {
public:
    void on_key(engine::platform::Key key) override;

    void on_mouse_move(engine::platform::MousePosition position) override;
};

class MainController final : public engine::core::Controller {
    void initialize() override;
    bool loop() override;
    void poll_events() override;

    void update() override;

    void begin_draw() override;
    void draw() override;
    void end_draw() override;

    void terminate() override;

    void update_camera();
    void update_fps_camera();
    void update_speed();
    void update_jump();

    void create_plane();
    void draw_plane();
    void destroy_plane();

    unsigned m_vbo_plane{0};
    unsigned m_vao_plane{0};
    bool m_cursor_enable{true};

    DirectionalLight m_dirlight{};
    void set_dirlight();

    SpotLight m_spotlight{};
    void set_spotlight();
    void turn_spotlight();

    void draw_tree();
    void draw_cabin();
    void draw_rifle();
    void draw_target();
    void draw_skybox();
};
}// app

#endif //MAINCONTROLLER_HPP
