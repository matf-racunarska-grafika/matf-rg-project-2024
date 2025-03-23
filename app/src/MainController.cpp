//
// Created by strahinjas on 3/18/25.
//

#include <MainController.hpp>
#include <imgui_internal.h>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>
#include <engine/core/Engine.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/graphics/Camera.hpp>

namespace my_project {
void MyController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    spdlog::info("Hello, from the MyController initialize");

    // Some debugging prints
    // auto resource_c = engine::core::Controller::get<engine::resources::ResourcesController>();
    // resource_c->print_loaded_textures();
    // auto specular_t = resource_c->texture("resources/models/backpack/specular.jpg", "resources/models/backpack/specular.jpg", engine::resources::TextureType::Specular, false);
    // for (auto &key: resource_c->get_m_textures() | std::views::keys) { spdlog::info(key.c_str()); }

    MyController::initalize_camera();
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
    if (platform->key(engine::platform::KEY_SPACE)
                .state_str() == "Pressed") { camera->move_camera(engine::graphics::Camera::UP, dt); }
    if (platform->key(engine::platform::KEY_LEFT_CONTROL)
                .state_str() == "Pressed") { camera->move_camera(engine::graphics::Camera::DOWN, dt); }
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
    // The ResourceController stores all my loaded models, textures, skyboxes, shaders so that I can access them when I need to draw
    this->draw_island_model();
    this->draw_light_source_birds();
    this->draw_skybox();


    //this->draw_light_cube();
}

void MyController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MyController::draw_light_source_birds() {
    auto resource_c = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto light_birds = resource_c->model("light_bird");

    auto graphics_c = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = resource_c->shader("light_source_birds");

    shader->use();
    shader->set_mat4("projection", graphics_c->projection_matrix());
    shader->set_mat4("view", graphics_c->camera()->view_matrix());
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.2f, 2.57f, 3.2f));
    model = glm::scale(model, glm::vec3(0.04f));
    shader->set_mat4("model", model);
    light_birds->draw(shader);

    auto model1 = glm::mat4(1.0f);
    model1 = glm::translate(model1, glm::vec3(-2.58f, -1.25f, 7.3f));
    model1 = glm::rotate(model1, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model1 = glm::scale(model1, glm::vec3(0.04f));
    shader->set_mat4("model", model1);
    light_birds->draw(shader);
}

void MyController::draw_island_model() {
    auto resource_c = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto island = resource_c->model("floating_island");

    auto graphics_c = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = resource_c->shader("lighting_scene");

    shader->use();
    shader->set_mat4("projection", graphics_c->projection_matrix());
    shader->set_mat4("view", graphics_c->camera()->view_matrix());
    auto model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.5f));
    shader->set_mat4("model", model);

    // Passing camera position
    shader->set_vec3("viewPos", graphics_c->camera()->Position);
    // Pass the parametars for my light source (The campfire which will act as a point light)
    shader->set_vec3("LightPoints[0].position", glm::vec3(0.2f, 3.0f, 3.3f));
    shader->set_vec3("LightPoints[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader->set_vec3("LightPoints[0].diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
    //shader->set_vec3("LightPoints[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader->set_float("LightPoints[0].constant", 1.0f);
    shader->set_float("LightPoints[0].linear", 0.14f);
    shader->set_float("LightPoints[0].quadratic", 0.07f);

    shader->set_vec3("LightPoints[1].position", glm::vec3(-2.55f, -0.85f, 7.33f));
    shader->set_vec3("LightPoints[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader->set_vec3("LightPoints[1].diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
    //shader->set_vec3("LightPoints[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader->set_float("LightPoints[1].constant", 1.0f);
    shader->set_float("LightPoints[1].linear", 0.22f);
    shader->set_float("LightPoints[1].quadratic", 0.20f);

    // Parametars for directional light
    shader->set_vec3("LightDirectional.direction", glm::vec3(1.0f, -1.0f, -1.0f));
    shader->set_vec3("LightDirectional.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader->set_vec3("LightDirectional.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));

    island->draw(shader);
}

void MyController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

void MyController::initalize_camera() {
    auto graphics_c = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics_c->camera();
    camera->Position = glm::vec3(0.0f, 0.0f, 3.0f);
}

void MyController::draw_model_dog() {
    auto resource_c = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto dog_model = resource_c->model("dog");

    auto graphics_c = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = resource_c->shader("lighting_scene");

    shader->use();
    shader->set_mat4("projection", graphics_c->projection_matrix());
    shader->set_mat4("view", graphics_c->camera()->view_matrix());
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(6.0f, -2.5f, 3.2f));
    model = glm::scale(model, glm::vec3(0.04f));
    shader->set_mat4("model", model);
    dog_model->draw(shader);
}

void MyController::draw_light_cube() {
    auto resource_c = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto light_birds = resource_c->model("cube");

    auto graphics_c = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = resource_c->shader("temp");

    shader->use();
    shader->set_mat4("projection", graphics_c->projection_matrix());
    shader->set_mat4("view", graphics_c->camera()->view_matrix());
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.2f, 3.0f, 3.3f));
    model = glm::scale(model, glm::vec3(0.04f));
    shader->set_mat4("model", model);
    light_birds->draw(shader);

    // auto model1 = glm::mat4(1.0f);
    // model1 = glm::rotate(model1, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // model1 = glm::translate(model1, glm::vec3(-5.8f, -0.8f, 5.15f));
    // model1 = glm::scale(model1, glm::vec3(0.04f));
    // shader->set_mat4("model", model1);
    // light_birds->draw(shader);
}

}
