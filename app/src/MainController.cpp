//
// Created by boce on 7/26/25.
//

#include "../../engine/libs/glad/include/glad/glad.h"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"

#include <MainController.hpp>
#include <spdlog/spdlog.h>

namespace app {

void MainPlatformEventObserver::on_key(engine::platform::Key key) { spdlog::info("Keyboard event: key={}, state={}", key.name(), key.state_str()); }

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) { spdlog::info("MousePosition: {} {}", position.x, position.y); }


void MainController::initialize() {
    spdlog::info("Main controller successfully initialized");
    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
    create_plane();
    engine::graphics::OpenGL::enable_depth_testing();
    engine::core::Controller::get<engine::graphics::GraphicsController>()->camera()->Position = glm::vec3(0.0f, 0.0f, 5.0f);
}

void MainController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enable = !m_cursor_enable;
        platform->set_enable_cursor(m_cursor_enable);
    }
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).state() == engine::platform::Key::State::JustPressed) { return false; }
    return true;
}

void MainController::update() {
    update_fps_camera();
    update_speed();
    update_jump();
}


void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KEY_W).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt); }

    if (platform->key(engine::platform::KEY_S).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt); }

    if (platform->key(engine::platform::KEY_A).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }

    if (platform->key(engine::platform::KEY_D).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt); }

    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
}

void MainController::update_fps_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();

    camera->move_fps_camera(
            platform->key(engine::platform::KEY_W).state() == engine::platform::Key::State::Pressed,
            platform->key(engine::platform::KEY_S).state() == engine::platform::Key::State::Pressed,
            platform->key(engine::platform::KEY_D).state() == engine::platform::Key::State::Pressed,
            platform->key(engine::platform::KEY_A).state() == engine::platform::Key::State::Pressed,
            dt
            );

    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
}

void MainController::update_speed() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();

    if (platform->key(engine::platform::KEY_LEFT_SHIFT).state() == engine::platform::Key::State::JustPressed) { camera->MovementSpeed = engine::graphics::Camera::RUN; }

    if (platform->key(engine::platform::KEY_LEFT_SHIFT).state() == engine::platform::Key::State::JustReleased) { camera->MovementSpeed = engine::graphics::Camera::WALK; }
}

void MainController::update_jump() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();

    if (!camera->Jump && platform->key(engine::platform::KEY_SPACE).state() == engine::platform::Key::State::JustPressed) {
        spdlog::info("JUMPED!!!");
        camera->Jump = true;
    }

    camera->update_jump(platform->dt());
}

void MainController::set_dirlight() {
    m_dirlight.direction = glm::vec3(0.2f, -3.0f, 0.3f);
    m_dirlight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    m_dirlight.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    m_dirlight.specular = glm::vec3(0.5f, 0.5f, 0.5f);
}

void MainController::set_spotlight() {
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    m_spotlight.position = camera->Position;
    m_spotlight.direction = camera->Front;
    m_spotlight.ambient = glm::vec3(0.0f);
    m_spotlight.diffuse = glm::vec3(1.0f);
    m_spotlight.specular = glm::vec3(1.0f);
    m_spotlight.inner_cut_off = glm::cos(glm::radians(10.5f));
    m_spotlight.outer_cut_off = glm::cos(glm::radians(12.0f));
    m_spotlight.constant = 1.0f;
    m_spotlight.linear = 0.09f;
    m_spotlight.quadratic = 0.032f;
    m_spotlight.lamp_on = 1;
}


void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
    set_dirlight();
    set_spotlight();
}

void MainController::draw() { draw_plane(); }

void MainController::end_draw() { engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers(); }


void MainController::terminate() { destroy_plane(); }


void MainController::create_plane() {
    float vertices[] = {
            -5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f,
            -5.0f, -0.5f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            5.0f, -0.5f, 5.0f, 0.0f, 1.0f, 0.0f, 5.0f, 0.0f,

            5.0f, -0.5f, 5.0f, 0.0f, 1.0f, 0.0f, 5.0f, 0.0f,
            5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 5.0f, 5.0f,
            -5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f
    };

    CHECKED_GL_CALL(glGenBuffers, 1, &m_vbo_plane);
    CHECKED_GL_CALL(glGenVertexArrays, 1, &m_vao_plane);

    CHECKED_GL_CALL(glBindVertexArray, m_vao_plane);
    CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_vbo_plane);

    CHECKED_GL_CALL(glGenBuffers, 1, &m_vbo_plane);
    CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_vbo_plane);
    CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    CHECKED_GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    CHECKED_GL_CALL(glEnableVertexAttribArray, 0);

    CHECKED_GL_CALL(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    CHECKED_GL_CALL(glEnableVertexAttribArray, 1);

    CHECKED_GL_CALL(glVertexAttribPointer, 2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    CHECKED_GL_CALL(glEnableVertexAttribArray, 2);

    CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}

void MainController::draw_plane() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("plane");
    auto texture = engine::core::Controller::get<engine::resources::ResourcesController>()->texture("ground");

    texture->bind(GL_TEXTURE0);

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("model", glm::mat4(1.0f));
    shader->set_int("texture_diffuse", 0);
    shader->set_vec3("viewPos", graphics->camera()->Position);

    m_dirlight.apply(shader, "dirlight");
    m_spotlight.apply(shader, "spotlight");

    CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_vbo_plane);
    CHECKED_GL_CALL(glBindVertexArray, m_vao_plane);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 6);
    CHECKED_GL_CALL(glBindVertexArray, 0);
    CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
}

void MainController::destroy_plane() {
    CHECKED_GL_CALL(glDeleteBuffers, 1, &m_vbo_plane);
    CHECKED_GL_CALL(glDeleteVertexArrays, 1, &m_vao_plane);
}


}// app