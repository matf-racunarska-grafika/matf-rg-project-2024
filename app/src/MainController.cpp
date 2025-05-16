//
// Created by ana on 5/13/25.
//

#include "../include/MainController.hpp"

#include "../../engine/libs/glfw/include/GLFW/glfw3.h"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx, position.dy);
}

void MainController::initialize() {
    spdlog::info("MainController initialized!");
    engine::graphics::OpenGL::enable_depth_testing();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());

    // da bi mis bio na sredini
    platform->set_enable_cursor(false);
    int width = platform->window()->width() / 2.0;
    int height = platform->window()->height() / 2.0;
    GLFWwindow *window_handle = platform->window()->handle_();
    glfwSetCursorPos(window_handle, width, height);

    // dirLight
    directionalLight.direction = glm::vec3(0.0f, -8.0f, 0.0f);
    directionalLight.ambient = glm::vec3(0.05f);
    directionalLight.diffuse = glm::vec3(1.0f, 0.7843f, 0.3921f) * 0.5f;
    directionalLight.specular = glm::vec3(0.2f, 0.2f, 0.2f) * 0.5f;

    redPointLight.position = glm::vec3(4.5, 4.7, 1.5);
    redPointLight.ambient = glm::vec3(0.4, 0.4, 0.2);
    redPointLight.diffuse = glm::vec3(2.0, 0.0, 0.0);
    redPointLight.specular = glm::vec3(0.5, 0.5, 0.5);
    redPointLight.constant = 1.0f;
    redPointLight.linear = 0.09f;
    redPointLight.quadratic = 0.09f;

}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
    return true;


}

void MainController::draw_car() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *car = resources->model("car");

    engine::resources::Shader *shader = resources->shader("car");

    shader->use();

    shader->set_vec3("directionalLight.direction", directionalLight.direction);
    shader->set_vec3("directionalLight.ambient", directionalLight.ambient);
    shader->set_vec3("directionalLight.diffuse", directionalLight.diffuse);
    shader->set_vec3("directionalLight.specular", directionalLight.specular);

    shader->set_float("material_shininess", 64.0f);
    shader->set_vec3("viewPosition", graphics->camera()->Position);

    shader->set_vec3("redPointLight.position", redPointLight.position);
    shader->set_vec3("redPointLight.ambient", redPointLight.ambient);
    shader->set_vec3("redPointLight.diffuse", redPointLight.diffuse);
    shader->set_vec3("redPointLight.specular", redPointLight.specular);
    shader->set_float("redPointLight.constant", redPointLight.constant);
    shader->set_float("redPointLight.linear", redPointLight.linear);
    shader->set_float("redPointLight.quadratic", redPointLight.quadratic);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shader->set_mat4("model", model);

    car->draw(shader);
}

void MainController::draw_traffic_light() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *traffic_light = resources->model("traffic_light");

    engine::resources::Shader *shader = resources->shader("traffic_light");

    shader->use();

    shader->set_vec3("directionalLight.direction", directionalLight.direction);
    shader->set_vec3("directionalLight.ambient", directionalLight.ambient);
    shader->set_vec3("directionalLight.diffuse", directionalLight.diffuse);
    shader->set_vec3("directionalLight.specular", directionalLight.specular);

    shader->set_float("material_shininess", 64.0f);
    shader->set_vec3("viewPosition", graphics->camera()->Position);

    shader->set_vec3("redPointLight.position", redPointLight.position);
    shader->set_vec3("redPointLight.ambient", redPointLight.ambient);
    shader->set_vec3("redPointLight.diffuse", redPointLight.diffuse);
    shader->set_vec3("redPointLight.specular", redPointLight.specular);
    shader->set_float("redPointLight.constant", redPointLight.constant);
    shader->set_float("redPointLight.linear", redPointLight.linear);
    shader->set_float("redPointLight.quadratic", redPointLight.quadratic);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.5f, 4.1f, 1.5f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.017f));
    shader->set_mat4("model", model);

    traffic_light->draw(shader);
}

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt); }
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt); }
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt); }


}

void MainController::update() { update_camera(); }

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw() {
    draw_car();
    draw_traffic_light();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}
}// app