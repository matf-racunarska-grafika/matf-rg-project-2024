//
// Created by matfrg on 7/14/25.
//

#include "MainController.hpp"
#include "../../engine/test/app/include/app/MainController.hpp"

#include "../../engine/libs/glfw/include/GLFW/glfw3.h"

#include <GuiController.hpp>
#include <LampEvent.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/util/Configuration.hpp>
#include <spdlog/spdlog.h>

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;

    void on_key(engine::platform::Key key) override;
};

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto main_controller = engine::platform::PlatformController::get<MainController>();
    if (key.id() == engine::platform::KeyId::KEY_B) {
        main_controller->lamp_event_handler->start_automatic_cycle();
        main_controller->lamp_event_handler->cycle_active = true;
    }
    if (key.id() == engine::platform::KeyId::KEY_N && key.state() == engine::platform::Key::State::Pressed) { main_controller->lamp_event_handler->cycle_manual_color(); }
}

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    if (!gui_controller->is_enabled()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }
}

void MainController::initialize() {
    spdlog::info("MainController initialized");
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    lamp_event_handler = new LampEvent();
    engine::graphics::OpenGL::enable_depth_testing();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
    return true;
}

void MainController::draw() {
    draw_floor();
    draw_graves();
    draw_lantern();
    draw_lamp();
    draw_dog();
    draw_car();
}

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw_floor() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *floor = resources->model("cobble");
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-7.0f, -10.0f, -9.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(15.0f, 1.0f, 13.0f));
    shader->set_mat4("model", model);
    floor->draw(shader);
}

void MainController::draw_lamp() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *lamp = resources->model("streetlamp");
    engine::resources::Shader *shader = resources->shader("lampShader");
    glm::vec3 lamp_color = glm::vec3(0.1f, 5.0f, 0.0f);

    lamp_event_handler->update_lamp(get<engine::platform::PlatformController>()->dt());

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("spot_light.direction", graphics->camera()->Front);
    shader->set_float("spot_light.cutOff", spot_light.cutOff);
    shader->set_float("spot_light.outer_cutOff", spot_light.outer_cutOff);
    shader->set_vec3("spot_light.diffuse", lamp_color);
    shader->set_vec3("spot_light.specular", spot_light.specular);
    shader->set_float("spot_light.linear", spot_light.linear);
    shader->set_float("spot_light.quadratic", spot_light.quadratic);
    shader->set_float("spot_light.shininess", spot_light.shininess);
    shader->set_vec3("lightIntensity", point_light.intensity);
    shader->set_vec3("cameraPos", graphics->camera()->Position);
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 newLanternPos = glm::vec3(-20.0f, -10.0f, 17.0f);
    shader->set_vec3("lantern_spot_light.position", newLanternPos);
    model = glm::translate(model, newLanternPos);
    model = glm::scale(model, glm::vec3(3.0f));
    glm::vec3 lanternForward = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 lanternDirection = glm::normalize(glm::mat3(model) * lanternForward);
    shader->set_vec3("lantern_spot_light.direction", lanternDirection);
    shader->set_mat4("model", model);
    lamp->draw(shader);
}

void MainController::draw_lantern() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *lantern = resources->model("lantern");
    engine::resources::Shader *shader = resources->shader("lanternShader");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("lantern_spot_light.direction", graphics->camera()->Front);
    shader->set_float("lantern_spot_light.cutOff", spot_light.cutOff);
    shader->set_float("lantern_spot_light.outer_cutOff", spot_light.outer_cutOff);
    shader->set_vec3("lantern_spot_light.diffuse", glm::vec3(5.0f, 0.0f, 0.0f));
    shader->set_vec3("lantern_spot_light.specular", spot_light.specular);
    shader->set_float("lantern_spot_light.linear", spot_light.linear);
    shader->set_float("lantern_spot_light.quadratic", spot_light.quadratic);
    shader->set_float("lantern_spot_light.shininess", spot_light.shininess);
    shader->set_vec3("lightIntensity", point_light.intensity);
    shader->set_vec3("cameraPos", graphics->camera()->Position);
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 newLanternPos = glm::vec3(49.0f, -9.0f, -20.0f);
    shader->set_vec3("lantern_spot_light.position", newLanternPos);
    model = glm::translate(model, newLanternPos);
    model = glm::scale(model, glm::vec3(4.0f));
    glm::vec3 lanternForward = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 lanternDirection = glm::normalize(glm::mat3(model) * lanternForward);
    shader->set_vec3("lantern_spot_light.direction", lanternDirection);
    shader->set_mat4("model", model);
    lantern->draw(shader);
}

void MainController::draw_dog() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *dog = resources->model("dog");
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(40.0f, -10.0f, -20.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    shader->set_mat4("model", model);

    dog->draw(shader);
}

void MainController::draw_car() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *car = resources->model("car");
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-18.0f, -9.5f, 12.9f));
    model = glm::scale(model, glm::vec3(4.0f));
    shader->set_mat4("model", model);
    car->draw(shader);
}

void MainController::draw_graves() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *grave = resources->model("grave");
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    float TRANSLATE_X_FACTOR = 9.0f;
    float TRANSLATE_Z_FACTOR = -7.0f;
    float GRAVE_COLS = 8, GRAVE_ROWS = 7;
    for (int i = 1; i < GRAVE_ROWS; i++) {
        for (int j = 1; j < GRAVE_COLS; j++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-5.0f + i * TRANSLATE_X_FACTOR, -9.0f, TRANSLATE_Z_FACTOR * (j + 4.0f)));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.0f));
            shader->set_mat4("model", model);
            grave->draw(shader);
        }
    }
}

void MainController::update() { update_camera(); }

void MainController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    if (gui_controller->is_enabled()) { return; }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    camera->MovementSpeed = 10.0;
    float dt = platform->dt();
    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt); }
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt); }
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt); }
    if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::UP, dt); }
    if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt); }
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}
}// app