//
// Created by boce on 7/26/25.
//

#include "../../engine/libs/glad/include/glad/glad.h"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"

#include <MainController.hpp>
#include <complex>
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
    set_instanced_tree();
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

    set_dirlight();
    set_spotlight();
    turn_spotlight();
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
    m_dirlight.direction = glm::vec3(-0.5f, -2.5f, -5.0f);
    m_dirlight.ambient = glm::vec3(0.08f, 0.08f, 0.08f);
    m_dirlight.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
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
}

void MainController::turn_spotlight() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_V).state() == engine::platform::Key::State::JustPressed) { m_spotlight.lamp_on = 1 - m_spotlight.lamp_on; }
}


void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw() {
    draw_instanced_tree();
    draw_plane();
    draw_cabin();
    draw_target();
    draw_rifle();

    draw_skybox();
}

void MainController::end_draw() { engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers(); }


void MainController::terminate() {
    destroy_plane();
    delete m_model_tree;
}


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

void MainController::draw_tree() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("tree");
    auto tree = engine::core::Controller::get<engine::resources::ResourcesController>()->model("tree");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    m_dirlight.apply(shader, "dirlight");
    m_spotlight.apply(shader, "spotlight");

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, -0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    shader->set_mat4("model", model);
    shader->set_float("shininess", 8.0f);
    shader->set_vec3("viewPos", graphics->camera()->Position);

    tree->draw(shader);
}

void MainController::draw_cabin() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("cabin");
    auto cabin = engine::core::Controller::get<engine::resources::ResourcesController>()->model("cabin1");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    m_dirlight.apply(shader, "dirlight");
    m_spotlight.apply(shader, "spotlight");

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -0.5f, 1.0f));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    shader->set_mat4("model", model);
    shader->set_float("shininess", 32.0f);
    shader->set_vec3("viewPos", graphics->camera()->Position);

    glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
    shader->set_mat3("invNormal", normal_matrix);

    cabin->draw(shader);
}

void MainController::draw_rifle() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("item");
    auto rifle = engine::core::Controller::get<engine::resources::ResourcesController>()->model("ak_47");

    glm::vec3 offset = glm::vec3(0.22f, -0.28f, -0.85f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, offset);
    model = glm::rotate(model, glm::radians(110.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));

    shader->use();
    shader->set_mat4("model", model);
    shader->set_float("shininess", 32.0f);

    shader->set_mat4("projection", graphics->projection_matrix());

    glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
    shader->set_mat3("invNormal", normal_matrix);

    m_dirlight.apply(shader, "dirlight");
    m_spotlight.apply(shader, "spotlight");

    shader->set_vec3("viewPos", graphics->camera()->Position);

    glDepthRange(0.0, 0.01);
    rifle->draw(shader);
    glDepthRange(0.0, 1.0);
}

void MainController::draw_target() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("target");
    auto target = engine::core::Controller::get<engine::resources::ResourcesController>()->model("target");

    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.2f, -0.5f, 0.3f));
    model = glm::scale(model, glm::vec3(0.11f, 0.11f, 0.11f));

    shader->set_mat4("model", model);
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_float("shininess", 32.0f);

    glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
    shader->set_mat3("invNormal", normal_matrix);

    m_dirlight.apply(shader, "dirlight");
    m_spotlight.apply(shader, "spotlight");

    shader->set_vec3("viewPos", graphics->camera()->Position);

    target->draw(shader);
}

void MainController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox_night");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox);
}

void MainController::set_instanced_tree() {
    glm::vec3 offsets[] = {
            glm::vec3(1.0f, -0.5f, 0.0f),
            glm::vec3(1.2f, -0.5f, 0.2f),
            glm::vec3(1.2f, -0.5f, -0.4f),
            glm::vec3(1.6f, -0.5f, -1.0f),
            glm::vec3(1.5f, -0.5f, -0.8f),
            glm::vec3(1.6f, -0.5f, -1.3f),
            glm::vec3(2.0f, -0.5f, 1.1f),
            glm::vec3(2.2f, -0.5f, 1.4f),
            glm::vec3(2.5f, -0.5f, 1.8f),
            glm::vec3(2.3f, -0.5f, 2.2f),
            glm::vec3(2.1f, -0.5f, -1.7f),
            glm::vec3(2.7f, -0.5f, -2.0f),
            glm::vec3(0.6f, -0.5f, -2.3),
            glm::vec3(0.8f, -0.5f, -1.8f),
            glm::vec3(0.2f, -0.5f, -2.9f)
    };

    m_amount_tree = 15;
    m_model_tree = new glm::mat4[m_amount_tree];
    glm::vec3 scale_factor = glm::vec3(0.5f, 0.5f, 0.5f);

    for (unsigned int i = 0; i < m_amount_tree; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, offsets[i]);
        model = glm::scale(model, scale_factor);
        m_model_tree[i] = model;
    }
}

void MainController::draw_instanced_tree() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("tree");
    auto tree = engine::core::Controller::get<engine::resources::ResourcesController>()->model("tree");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    m_dirlight.apply(shader, "dirlight");
    m_spotlight.apply(shader, "spotlight");

    shader->set_float("shininess", 8.0f);
    shader->set_vec3("viewPos", graphics->camera()->Position);

    graphics->instanced_draw(tree, shader, m_model_tree, m_amount_tree);
}


}// app