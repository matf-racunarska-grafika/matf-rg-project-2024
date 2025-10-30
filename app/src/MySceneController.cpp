//
// Created by matija on 10/12/25.
//

#include "../include/MySceneController.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <LightSwarm.hpp>
#include <MyGUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <glm/gtx/quaternion.hpp>
#include <random>


namespace app {


void ScenePlatformEventObserver::on_key(engine::platform::Key key) {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (engine::platform::KEY_SPACE == key.id()) {
        const auto myscene = engine::core::Controller::get<app::MySceneController>();
        myscene->start_animation();
    }
}
void MySceneController::set_dim(float brightness) {
    m_scene.dim_lights(brightness);
}
void MySceneController::start_animation() {
    m_duration_timer.startTimer(m_duration_amount);
}
void MySceneController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    auto observer = std::make_unique<ScenePlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));

    m_scene=Scene();
    m_delay_timer=Timer();
    m_duration_timer=Timer();
}

bool MySceneController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) {
        return false;
    }
    return true;
}

void MySceneController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1)
                .state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);

    }
}

void MySceneController::update() {
    update_camera();
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto ft=platform->frame_time();

    if (m_delay_timer.isEnabled()) {
        if (!m_delay_timer.update(ft.dt)) {
            m_duration_timer.startTimer(m_duration_amount);
            m_scene.toggle_swarm();
        }
    }
    if (m_delay_timer.isEnabled()) {
        if (!m_delay_timer.update(ft.dt)) {
            m_scene.toggle_swarm();
        }
    }
    m_scene.update(ft.dt, std::sin(ft.current));

}


void MySceneController::update_camera() {
    auto gui = engine::core::Controller::get<app::MyGUIController>();
    if (gui->is_enabled()) {
         return;
    }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();
    if (platform->key(engine::platform::KEY_W)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
                }
    if (platform->key(engine::platform::KEY_S)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
                }
    if (platform->key(engine::platform::KEY_A)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
                }
    if (platform->key(engine::platform::KEY_D)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
                }
    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

void MySceneController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();

}

void MySceneController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    std::vector<engine::graphics::Light> lights = m_scene.get_lights();
    graphics->deferred_filter().setUpCanvas(lights);
    m_scene.draw_static_scene(graphics->deferred_filter().geometry_shader());
    engine::resources::Shader * drawing_shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("deferred_bloom_aware_render");
    graphics->bloom_filter().setUpCanvas();
    graphics->deferred_filter().render(drawing_shader);
    engine::graphics::OpenGL::bindFrameBuffer(0);
    graphics->bloom_filter().applyBloom();
    graphics->deferred_filter().blitDepth(graphics->perspective_params().Width, graphics->perspective_params().Height, 0);

    //special shaders?
    m_scene.draw_lights();
    m_scene.draw_skybox();

}

void MySceneController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

} // namespace app
