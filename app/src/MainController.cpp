//
// Created by cvnpko on 7/14/25.
//

#include <MainController.hpp>

namespace app {
void MainController::initialize() { engine::graphics::OpenGL::enable_depth_testing(); }

bool MainController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) { return false; }
    return true;
}

void MainController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    }
    if (platform->key(engine::platform::KEY_F2).state() == engine::platform::Key::State::JustPressed) { m_is_day = !m_is_day; }
}

void MainController::update() { update_camera(); }

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw() {
    draw_dunes();
    draw_pyramids();
    draw_skybox();
}

void MainController::end_draw() { engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers(); }

void MainController::draw_dunes() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    auto dunes = engine::core::Controller::get<engine::resources::ResourcesController>()->model("dunes");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(15.0f, 2.0f, 15.0f));
    model = glm::translate(model, glm::vec3(0.0f, -1.1f, 0.0f));
    shader->set_mat4("model", model);
    dunes->draw(shader);
}

void MainController::draw_pyramids() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    auto pyramid = engine::core::Controller::get<engine::resources::ResourcesController>()->model("pyramid");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model1 = glm::mat4(1.0f);
    model1 = glm::scale(model1, glm::vec3(15.7f));
    model1 = glm::translate(model1, glm::vec3(3.15f, -0.098f, 1.8f));
    shader->set_mat4("model", model1);
    pyramid->draw(shader);

    glm::mat4 model2 = glm::mat4(1.0f);
    model2 = glm::scale(model2, glm::vec3(15.3f));
    model2 = glm::translate(model2, glm::vec3(0.0f, -0.09f, 3.0f));
    shader->set_mat4("model", model2);
    pyramid->draw(shader);

    glm::mat4 model3 = glm::mat4(1.0f);
    model3 = glm::scale(model3, glm::vec3(7.0f));
    model3 = glm::translate(model3, glm::vec3(-2.0f, -0.081f, 1.0f));
    shader->set_mat4("model", model3);
    pyramid->draw(shader);

}

void MainController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox(m_is_day ? "day" : "night");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();
    if (platform->key(engine::platform::KEY_W).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::FORWARD, 6 * dt); }
    if (platform->key(engine::platform::KEY_S).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::BACKWARD, 6 * dt); }
    if (platform->key(engine::platform::KEY_A).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::LEFT, 6 * dt); }
    if (platform->key(engine::platform::KEY_D).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::RIGHT, 6 * dt); }
    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

}// app