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
    directionalLight.direction = glm::vec3(0.0f, -8.0f, -5.0f);
    directionalLight.ambient = glm::vec3(0.05f);
    directionalLight.diffuse = glm::vec3(1.0f, 0.7843f, 0.3921f) * 0.5f;
    directionalLight.specular = glm::vec3(0.2f, 0.2f, 0.2f) * 0.5f;

    redPointLight.position = glm::vec3(4.5, 4.7, -3.5);
    redPointLight.ambient = glm::vec3(0.4, 0.4, 0.2);
    redPointLight.diffuse = glm::vec3(2.0, 0.0, 0.0);
    redPointLight.specular = glm::vec3(0.5, 0.5, 0.5);
    redPointLight.constant = 1.0f;
    redPointLight.linear = 0.09f;
    redPointLight.quadratic = 0.09f;

    // on startup yellow is turned off
    yellowPointLight.position = glm::vec3(4.5, 4.4, -3.5);
    yellowPointLight.ambient = glm::vec3(0.0, 0.0, 0.0);
    yellowPointLight.diffuse = glm::vec3(0.0, 0.0, 0.0);
    yellowPointLight.specular = glm::vec3(0.0, 0.0, 0.0);
    yellowPointLight.constant = 1.0f;
    yellowPointLight.linear = 0.09f;
    yellowPointLight.quadratic = 0.09f;

    // on startup green is turned off
    greenPointLight.position = glm::vec3(4.5, 4.05, -3.5);
    greenPointLight.ambient = glm::vec3(0.0, 0.0, 0.0);
    greenPointLight.diffuse = glm::vec3(0.0, 0.0, 0.0);
    greenPointLight.specular = glm::vec3(0.0, 0.0, 0.0);
    greenPointLight.constant = 1.0f;
    greenPointLight.linear = 0.09f;
    greenPointLight.quadratic = 0.09f;

}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
    return true;

}

void MainController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed && !transition_started) {
        turn_off(greenPointLight);
        turn_off(yellowPointLight);
        turn_on(redPointLight, 0);

        transition_on = true;
        red_on = false;
        blinking_yellow = false;
    }
    if (platform->key(engine::platform::KeyId::KEY_R).state() == engine::platform::Key::State::JustPressed && !transition_started) {
        turn_off(greenPointLight);
        turn_off(yellowPointLight);

        transition_on = false;
        red_on = true;
        blinking_yellow = false;
    }

    if (platform->key(engine::platform::KeyId::KEY_Y).state() == engine::platform::Key::State::JustPressed && !transition_started && !blinking_yellow) {
        turn_off(redPointLight);
        turn_off(greenPointLight);
        turn_on(yellowPointLight, 1);
        state = 1;

        transition_on = false;
        red_on = false;
        blinking_yellow = true;
        yellow_on = true;
        last_toggle_time = std::chrono::steady_clock::now();
    }
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

void MainController::update_lights_go() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();

    if (transition_on) {
        if (!transition_started) {
            transition_started = true;
            time_since_transition = 0.0f;
            turn_on(redPointLight, 0);
            state = 0;
            //turn_on(yellowPointLight, 1);
            spdlog::info("red on ,wait for 3 second......");
        }

        if (transition_started) {
            time_since_transition += dt;

            if (time_since_transition >= 1.0f) {
                turn_on(yellowPointLight, 1);
                state = 3;
            }

            if (time_since_transition >= 3.0f) {
                transition_started = false;
                turn_off(redPointLight);
                turn_off(yellowPointLight);
                turn_on(greenPointLight, 2);
                state = 2;
                spdlog::info("3 seconds passed, green on....");
                transition_on = false;
            }
        }
    }
}

void MainController::update_lights_red() {
    if (red_on) {
        turn_on(redPointLight, 0);
        state = 0;
    }
}

void MainController::update_blinking_yellow() {
    if (blinking_yellow) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_toggle_time).count() > 500) {
            yellow_on = !yellow_on;
            if (yellow_on) {
                turn_on(yellowPointLight, 1);
                state = 1;
            } else {
                turn_off(yellowPointLight);
                state = -1;
            }
            last_toggle_time = now;
        }
    }
}

void MainController::update() {
    update_camera();
    update_lights_go();
    update_lights_red();
    update_blinking_yellow();
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

    shader->set_vec3("yellowPointLight.position", yellowPointLight.position);
    shader->set_vec3("yellowPointLight.ambient", yellowPointLight.ambient);
    shader->set_vec3("yellowPointLight.diffuse", yellowPointLight.diffuse);
    shader->set_vec3("yellowPointLight.specular", yellowPointLight.specular);
    shader->set_float("yellowPointLight.constant", yellowPointLight.constant);
    shader->set_float("yellowPointLight.linear", yellowPointLight.linear);
    shader->set_float("yellowPointLight.quadratic", yellowPointLight.quadratic);

    shader->set_vec3("greenPointLight.position", greenPointLight.position);
    shader->set_vec3("greenPointLight.ambient", greenPointLight.ambient);
    shader->set_vec3("greenPointLight.diffuse", greenPointLight.diffuse);
    shader->set_vec3("greenPointLight.specular", greenPointLight.specular);
    shader->set_float("greenPointLight.constant", greenPointLight.constant);
    shader->set_float("greenPointLight.linear", greenPointLight.linear);
    shader->set_float("greenPointLight.quadratic", greenPointLight.quadratic);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
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

    shader->set_vec3("yellowPointLight.position", yellowPointLight.position);
    shader->set_vec3("yellowPointLight.ambient", yellowPointLight.ambient);
    shader->set_vec3("yellowPointLight.diffuse", yellowPointLight.diffuse);
    shader->set_vec3("yellowPointLight.specular", yellowPointLight.specular);
    shader->set_float("yellowPointLight.constant", yellowPointLight.constant);
    shader->set_float("yellowPointLight.linear", yellowPointLight.linear);
    shader->set_float("yellowPointLight.quadratic", yellowPointLight.quadratic);

    shader->set_vec3("greenPointLight.position", greenPointLight.position);
    shader->set_vec3("greenPointLight.ambient", greenPointLight.ambient);
    shader->set_vec3("greenPointLight.diffuse", greenPointLight.diffuse);
    shader->set_vec3("greenPointLight.specular", greenPointLight.specular);
    shader->set_float("greenPointLight.constant", greenPointLight.constant);
    shader->set_float("greenPointLight.linear", greenPointLight.linear);
    shader->set_float("greenPointLight.quadratic", greenPointLight.quadratic);

    shader->set_int("state", state);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.5f, 4.1f, -3.5f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.017f));
    shader->set_mat4("model", model);

    traffic_light->draw(shader);
}

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw() {
    draw_car();
    draw_traffic_light();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MainController::turn_off(PointLight &light) {
    light.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    light.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
    light.specular = glm::vec3(0.0f, 0.0f, 0.0f);
}

void MainController::turn_on(PointLight &light, int color) {
    // color => 0 - red, 1 - yellow, 2 - green
    light.ambient = glm::vec3(0.4f, 0.4f, 0.2f);
    light.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    if (color == 0) { light.diffuse = glm::vec3(2.0f, 0.0f, 0.0f); } else if (color == 1) { light.diffuse = glm::vec3(2.0f, 2.0f, 0.0f); } else if (color == 2) { light.diffuse = glm::vec3(0.0f, 2.0f, 0.0f); }

}
}// app