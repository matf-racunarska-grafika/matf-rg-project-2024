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

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

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
    shader->set_int("texture1", 0);

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