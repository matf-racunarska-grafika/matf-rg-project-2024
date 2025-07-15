//
// Created by cvnpko on 7/14/25.
//
#include <MainController.hpp>
#include <MainEventController.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace app {
void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    auto graphics = engine::graphics::GraphicsController::get<engine::graphics::GraphicsController>();
    engine::graphics::PerspectiveMatrixParams &perspective_matrix = graphics->perspective_params();
    perspective_matrix.Far = 500.0f;
}

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
}

void MainController::update() { update_camera(); }

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw() {
    draw_dunes();
    draw_pyramids();
    // draw_sphinx();
    draw_camels();
    draw_moon();
    draw_sun();
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
    model = glm::translate(model, glm::vec3(0.0f, -2.1f, 0.0f));
    model = glm::scale(model, glm::vec3(15.0f, 2.0f, 15.0f));
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
    model1 = glm::translate(model1, glm::vec3(50.00f, -1.098f, 0.0f));
    model1 = glm::scale(model1, glm::vec3(18.84f));
    shader->set_mat4("model", model1);
    pyramid->draw(shader);

    glm::mat4 model2 = glm::mat4(1.0f);
    model2 = glm::translate(model2, glm::vec3(0.0f, -1.09f, -50.0f));
    model2 = glm::scale(model2, glm::vec3(18.36f));
    shader->set_mat4("model", model2);
    pyramid->draw(shader);

    glm::mat4 model3 = glm::mat4(1.0f);
    model3 = glm::translate(model3, glm::vec3(-50.0f, -1.081f, -90.0f));
    model3 = glm::scale(model3, glm::vec3(8.4f));
    shader->set_mat4("model", model3);
    pyramid->draw(shader);
}

void MainController::draw_sphinx() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    auto sphinx = engine::core::Controller::get<engine::resources::ResourcesController>()->model("sphinx");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5.0f, 7.0f, 50.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f));
    shader->set_mat4("model", model);
    sphinx->draw(shader);
}

void MainController::draw_camels() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    auto camel = engine::core::Controller::get<engine::resources::ResourcesController>()->model("camel");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model1 = glm::mat4(1.0f);
    model1 = glm::translate(model1, glm::vec3(-60.0f, -0.58f, 40.0f));
    model1 = glm::scale(model1, glm::vec3(5.0f));
    shader->set_mat4("model", model1);
    camel->draw(shader);
    glm::mat4 model2 = glm::mat4(1.0f);
    model2 = glm::translate(model2, glm::vec3(-50.0f, -0.65f, 50.0f));
    model2 = glm::scale(model2, glm::vec3(5.0f));
    shader->set_mat4("model", model2);
    camel->draw(shader);
}

void MainController::draw_moon() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    auto main_event_controller = engine::core::Controller::get<app::MainEventController>();
    auto moon = engine::core::Controller::get<engine::resources::ResourcesController>()->model("moon");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(main_event_controller->get_event_degree()), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, -300.0f, 0.0f));
    model = glm::scale(model, glm::vec3(15.0f));
    shader->set_mat4("model", model);
    moon->draw(shader);
}

void MainController::draw_sun() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    auto main_event_controller = engine::core::Controller::get<app::MainEventController>();
    auto sun = engine::core::Controller::get<engine::resources::ResourcesController>()->model("sun");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(main_event_controller->get_event_degree()), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, 300.0f, 0.0f));
    model = glm::scale(model, glm::vec3(15.0f));
    shader->set_mat4("model", model);
    sun->draw(shader);
}

void MainController::draw_skybox() {
    auto main_event_controller = engine::core::Controller::get<app::MainEventController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox(main_event_controller->is_day() ? "day" : "night");
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