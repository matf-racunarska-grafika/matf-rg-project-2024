//
// Created by matija on 10/12/25.
//

#include <MySceneController.hpp>
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

void ScenePlatformEventObserver::on_window_resize(int width, int height) {
    auto scene = engine::core::Controller::get<app::MySceneController>();
    scene->set_window_size(width,height);
}

void MySceneController::set_dim(float brightness) {
    m_scene.dim_lights(brightness);
}

void MySceneController::start_animation() {
    m_delay_timer.start_timer(m_delay_amount);
}

void MySceneController::set_window_size(int width, int height) {
    m_scene.set_width_height(width, height);
}

void MySceneController::set_threshold(float threshold) {
    m_scene.set_threshold(threshold);
}

void MySceneController::set_bloom_intensity(float brightness) {
    m_scene.set_bloom_intensity(brightness);
}

void MySceneController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    auto observer = std::make_unique<ScenePlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->set_enable_cursor(false);

    m_scene=Scene();
    m_scene.init_scene();
    m_scene.set_swarm_enabled(false);
    m_delay_timer=Timer();
    m_duration_timer=Timer();
    m_delay_timer.restart_timer();
    m_duration_timer.restart_timer();

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
    if (platform->key(engine::platform::KEY_B)
                .state() == engine::platform::Key::State::JustPressed) {
        scene()->set_swarm_enabled(true);
    }
}

void MySceneController::update() {
    update_camera();
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto ft=platform->frame_time();


    if (m_delay_timer.is_enabled()) {
        if (!m_delay_timer.update(ft.dt)) {
            m_duration_timer.start_timer(m_duration_amount);
            m_scene.set_swarm_enabled(true);
        }
    }
    if (m_duration_timer.is_enabled()) {
        if (!m_duration_timer.update(ft.dt)) {
            m_scene.set_swarm_enabled(false);
        }
    }
    m_scene.update(ft);

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
    m_scene.draw();
    //m_scene.draw_bloom();
}

void MySceneController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

} // namespace app
