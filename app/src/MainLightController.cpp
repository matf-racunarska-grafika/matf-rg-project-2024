//
// Created by cvnpko on 7/17/25.
//

#include <MainLightController.hpp>

namespace app {
void MainLightController::setLight(engine::resources::Shader *shader) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto main_event_controller = engine::core::Controller::get<app::MainEventController>();

    shader->set_vec3("pointLightSun.position", glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(angle),
                                                                     glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(m_point_light_sun.position, 1.0)));
    shader->set_vec3("pointLightSun.ambient", m_point_light_sun.ambient);
    shader->set_vec3("pointLightSun.diffuse", m_point_light_sun.diffuse);
    shader->set_vec3("pointLightSun.specular", m_point_light_sun.specular);
    shader->set_float("pointLightSun.constant", m_point_light_sun.constant);
    shader->set_float("pointLightSun.linear", m_point_light_sun.linear);
    shader->set_float("pointLightSun.quadratic", m_point_light_sun.quadratic);

    shader->set_vec3("pointLightMoon.position", glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(angle),
                                                                      glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(m_point_light_moon.position, 1.0)));
    shader->set_vec3("pointLightMoon.ambient", m_point_light_moon.ambient);
    shader->set_vec3("pointLightMoon.diffuse", m_point_light_moon.diffuse);
    shader->set_vec3("pointLightMoon.specular", m_point_light_moon.specular);
    shader->set_float("pointLightMoon.constant", m_point_light_moon.constant);
    shader->set_float("pointLightMoon.linear", m_point_light_moon.linear);
    shader->set_float("pointLightMoon.quadratic", m_point_light_moon.quadratic);

    shader->set_vec3("spotLight.position", graphics->camera()->Position);
    shader->set_vec3("spotLight.direction", graphics->camera()->Front);
    shader->set_vec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    shader->set_vec3("spotLight.diffuse", m_spotlight_enabled ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));
    shader->set_vec3("spotLight.specular", m_spotlight_enabled ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));
    shader->set_float("spotLight.constant", 1.0f);
    shader->set_float("spotLight.linear", 0.09f);
    shader->set_float("spotLight.quadratic", 0.032f);
    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}

void MainLightController::initialize() {
    m_point_light_sun.position = glm::vec3(0.0f, 300.0f, 0.0f);
    m_point_light_sun.ambient = m_sun_light * m_sun_light_coeff * m_ambient_coeff;
    m_point_light_sun.diffuse = m_sun_light * m_sun_light_coeff;
    m_point_light_sun.specular = m_sun_light * m_sun_light_coeff;
    m_point_light_sun.constant = 1.0f;
    m_point_light_sun.linear = 0.0009f;
    m_point_light_sun.quadratic = 0.0000004f;

    m_point_light_moon.position = glm::vec3(0.0f, -300.0f, 0.0f);
    m_point_light_moon.ambient = m_moon_light * m_moon_light_coeff * m_ambient_coeff;
    m_point_light_moon.diffuse = m_moon_light * m_moon_light_coeff;
    m_point_light_moon.specular = m_moon_light * m_moon_light_coeff;
    m_point_light_moon.constant = 1.0f;
    m_point_light_moon.linear = 0.0009f;
    m_point_light_moon.quadratic = 0.0000004f;
}

bool MainLightController::loop() { return true; }

void MainLightController::poll_events() {
    auto main_event_controller = engine::core::Controller::get<app::MainEventController>();
    angle = main_event_controller->get_event_angle();
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F).state() == engine::platform::Key::State::JustPressed) { m_spotlight_enabled = !m_spotlight_enabled; }
}

void MainLightController::update() {}

void MainLightController::begin_draw() {}

void MainLightController::draw() {
    draw_moon();
    draw_sun();
}

void MainLightController::draw_moon() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("sun_moon_shader");
    auto main_event_controller = engine::core::Controller::get<app::MainEventController>();
    auto moon = engine::core::Controller::get<engine::resources::ResourcesController>()->model("moon");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, -300.0f, 0.0f));
    model = glm::scale(model, glm::vec3(15.0f));
    shader->set_mat4("model", model);
    shader->set_vec3("light", m_moon_light);
    shader->set_float("light_coeff", m_moon_light_coeff);
    moon->draw(shader);
}

void MainLightController::draw_sun() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("sun_moon_shader");
    auto sun = engine::core::Controller::get<engine::resources::ResourcesController>()->model("sun");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, 300.0f, 0.0f));
    model = glm::scale(model, glm::vec3(15.0f));
    shader->set_mat4("model", model);
    shader->set_vec3("light", m_sun_light);
    shader->set_float("light_coeff", m_sun_light_coeff);
    sun->draw(shader);
}

void MainLightController::end_draw() {}
}