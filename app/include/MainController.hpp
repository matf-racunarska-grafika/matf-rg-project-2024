//
// Created by boce on 7/26/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <Lights.hpp>
#include <Target.hpp>

struct RayCast {
    glm::vec3 origin;
    glm::vec3 dir;
};

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

    RayCast m_raycast{};
    void update_raycast();

    void create_plane();
    void draw_plane();
    void destroy_plane();

    unsigned m_vbo_plane{0};
    unsigned m_vao_plane{0};
    bool m_cursor_enable{true};

    DirectionalLight m_dirlight{};
    void set_dirlight();

    DirectionalLight m_rifle_dirlight{};
    void set_rifle_dirlight();

    SpotLight m_spotlight{};
    void set_spotlight();
    void update_spotlight();

    int m_amount_tree{};
    glm::mat4 *m_model_tree{};
    void set_instanced_tree();
    void draw_instanced_tree();

    std::vector<Target> m_targets{};
    void set_targets();
    void draw_targets();
    void awake_targets();
    void update_targets();
    void create_boundingbox_targets();
    void check_boundingbox_intersects();


    void draw_tree();
    void draw_cabin();
    void draw_rifle();
    void draw_target();
    void draw_skybox();

    unsigned m_chrosshair_vao{};
    void set_crosshair();
    void draw_crosshair();
};
}// app

#endif //MAINCONTROLLER_HPP
