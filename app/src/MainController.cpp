//
// Created by matfrg on 11/1/25.
//
#include <MainController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

namespace my_project {
void MyController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    spdlog::info("Hello, from the MyController initialize");
}

bool MyController::loop() {
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
    engine::graphics::OpenGL::clear_buffers();
}

void MyController::draw() {
    draw_test_model();
    draw_scene_with_lights();
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

void MyController::draw_scene_with_lights() {
    using namespace engine;
    auto resource_c = core::Controller::get<resources::ResourcesController>();
    auto graphics_c = core::Controller::get<graphics::GraphicsController>();

    auto model = resource_c->model("backpack");
    auto shader = resource_c->shader("lighting_shader");

    shader->use();

    shader->set_mat4("projection", graphics_c->projection_matrix());
    shader->set_mat4("view", graphics_c->camera()->view_matrix());

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, -1.0f, -3.0f));
    modelMat = glm::scale(modelMat, glm::vec3(1.0f));
    shader->set_mat4("model", modelMat);

    shader->set_vec3("viewPos", graphics_c->camera()->Position);

    shader->set_vec3("dirLightDir", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->set_vec3("dirLightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // belo svetlo

    static glm::vec3 pointPos(2.0f, 2.0f, 2.0f); // početna pozicija svetla
    shader->set_vec3("pointLightPos", pointPos);
    shader->set_vec3("pointLightColor", glm::vec3(1.0f, 0.6f, 0.6f)); // toplo svetlo

    shader->set_vec3("objectColor", glm::vec3(1.0f, 0.9f, 0.8f));


    auto platform = core::Controller::get<platform::PlatformController>();

    if (platform->key(platform::KEY_1).state_str() == "Pressed")
        shader->set_vec3("pointLightColor", glm::vec3(1.0f, 0.0f, 0.0f)); // crveno
    if (platform->key(platform::KEY_2).state_str() == "Pressed")
        shader->set_vec3("pointLightColor", glm::vec3(0.0f, 1.0f, 0.0f)); // zeleno
    if (platform->key(platform::KEY_3).state_str() == "Pressed")
        shader->set_vec3("pointLightColor", glm::vec3(0.0f, 0.0f, 1.0f)); // plavo

    // Pomeri svetlo WASD
    float moveSpeed = 2.0f * platform->dt();
    if (platform->key(platform::KEY_I).state() == platform::Key::State::Pressed)
        pointPos.z -= moveSpeed;
    if (platform->key(platform::KEY_K).state() == platform::Key::State::Pressed)
        pointPos.z += moveSpeed;
    if (platform->key(platform::KEY_J).state() == platform::Key::State::Pressed)
        pointPos.x -= moveSpeed;
    if (platform->key(platform::KEY_L).state() == platform::Key::State::Pressed)
        pointPos.x += moveSpeed;
    if (platform->key(platform::KEY_U).state() == platform::Key::State::Pressed)
        pointPos.y += moveSpeed;
    if (platform->key(platform::KEY_O).state() == platform::Key::State::Pressed)
        pointPos.y -= moveSpeed;

    model->draw(shader);

    auto lightShader = resource_c->shader("light_cube");
    auto sphere = resource_c->model("sphere");

    lightShader->use();
    lightShader->set_mat4("projection", graphics_c->projection_matrix());
    lightShader->set_mat4("view", graphics_c->camera()->view_matrix());

    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, pointPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.1f));
    lightShader->set_mat4("model", lightModel);

    sphere->draw(lightShader);
}
}