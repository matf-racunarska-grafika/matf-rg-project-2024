//
// Created by strahinjas on 3/18/25.
//

#include <MainController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

namespace my_project {
void MyController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    spdlog::info("Hello, from the MyController initialize");
}

bool MyController::loop() {
    // If the user presses the escape key the application should close
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).state() == engine::platform::Key::State::JustReleased) { return false; }
    // if (platform->key(engine::platform::KEY_ESCAPE).state_str() == "JustReleased") { return false; }
    return true;
}

void MyController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1).state_str() == "JustReleased") {
        enable_cursor = !enable_cursor;
        platform->set_enable_cursor(enable_cursor);
    }
}

void MyController::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();
    if (platform->key(engine::platform::KEY_W)
                .state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt); }
    if (platform->key(engine::platform::KEY_S)
                .state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt); }
    if (platform->key(engine::platform::KEY_A)
                .state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }
    if (platform->key(engine::platform::KEY_D)
                .state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt); }
    if (platform->key(engine::platform::KEY_SPACE).state_str() == "Pressed") { camera->move_camera(engine::graphics::Camera::UP, dt); }
    if (platform->key(engine::platform::KEY_LEFT_CONTROL).state_str() == "Pressed") { camera->move_camera(engine::graphics::Camera::DOWN, dt); }
    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

void MyController::begin_draw() {
    // Let`s clear the color buffers so that the new colors can be drawn to the pixels
    engine::graphics::OpenGL::clear_buffers();
}

void MyController::draw() {
    // I want to draw my backpack model
    // The model is loaded in automatically when I call My applications run (with the initialize method specifically)
    // The ResourceController stores all my loaded models, textures, sky boxes, shaders so that I can access them when I
    // need to draw
    draw_test_model();

    // Now I want to draw a cube, this cube will represent the point light in My scene
    draw_light_cube();

}

void MyController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MyController::draw_test_model() {
    auto resource_c = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto backpack = resource_c->model("backpack");
    auto shader = resource_c->shader("basic_backpack");

    auto graphics = engine::graphics::GraphicsController::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    shader->set_mat4("model", glm::mat4(1.0f));
    backpack->draw(shader);
}

void MyController::draw_light_cube() {
    auto resource_c = engine::core::Controller::get<engine::resources::ResourcesController>();

    auto cube = resource_c->model("cube");
    auto shader = resource_c->shader("light_cube");
    auto graphics_c = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_mat4("projection", graphics_c->projection_matrix());
    shader->set_mat4("view", graphics_c->camera()->view_matrix());
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 2.0f, 2.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    shader->set_mat4("model", model);
    cube->draw(shader);
}

}
