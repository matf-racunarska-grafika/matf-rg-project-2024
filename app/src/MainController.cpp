#include "MainController.hpp"

#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/graphics/GraphicsController.hpp>

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        camera->rotate_camera(position.dx, position.dy);
    }

    void on_scroll(engine::platform::MousePosition position) override {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        camera->zoom(position.scroll);
        graphics->perspective_params().FOV = glm::radians(camera->Zoom);
    }

    void on_window_resize(int width, int height) override {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->perspective_params().Width = static_cast<float>(width);
        graphics->perspective_params().Height = static_cast<float>(height);
    }
};

void MainController::initialize() {
    spdlog::info("MainController initialized");

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::graphics::OpenGL::enable_depth_testing();

    m_skyboxShader = resources->shader("skybox");
    m_basicShader = resources->shader("basic");
    m_skybox = resources->skybox("dikhololo_night");

    m_tree = resources->model("tree");
    m_campfire = resources->model("campfire");
    m_tent = resources->model("tent");
    m_stone = resources->model("stone");
    m_lantern = resources->model("lantern");

    spdlog::info("All models requested from ResourcesController.");

    // Camera start
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    graphics->perspective_params().FOV = glm::radians(100.0f);
    camera->Position = glm::vec3(0.0f, 2.5f, 3.0f);
    camera->Front = glm::normalize(glm::vec3(0.0f, -0.3f, -1.0f));
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) return false;
    return true;
}

void MainController::update() { update_camera(); }

void MainController::update_camera() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    float dt = platform->dt();


    bool sprint = platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).is_down() ||
                  platform->key(engine::platform::KeyId::KEY_RIGHT_SHIFT).is_down();
    float speed = m_moveSpeed * (sprint ? m_sprintScale : 1.0f);


    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt * speed);
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt * speed);
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt * speed);
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt * speed);
    if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt * speed);
    if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt * speed);
    if (platform->key(engine::platform::KeyId::KEY_LEFT).is_down()) camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt * speed);
    if (platform->key(engine::platform::KeyId::KEY_RIGHT).is_down()) camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt * speed);


    if (platform->key(engine::platform::KeyId::KEY_Q).is_down()) camera->Position += glm::vec3(0.0f, -speed * dt, 0.0f);
    if (platform->key(engine::platform::KeyId::KEY_E).is_down()) camera->Position += glm::vec3(0.0f, speed * dt, 0.0f);
}

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();


    if (m_skyboxShader && m_skybox) { graphics->draw_skybox(m_skyboxShader, m_skybox); }


    if (!m_basicShader) return;

    m_basicShader->use();
    m_basicShader->set_mat4("projection", graphics->projection_matrix());
    m_basicShader->set_mat4("view", graphics->camera()->view_matrix());


    glm::vec3 lightPos = glm::vec3(1.5f, 1.2f, -6.0f);
    m_basicShader->set_vec3("uLightPos", lightPos);
    m_basicShader->set_vec3("uViewPos", graphics->camera()->Position);
    auto draw_model = [&](engine::resources::Model *mdl,
                          const glm::vec3 &pos,
                          const glm::vec3 &scale = glm::vec3(1.0f),
                          float rotYdeg = 0.0f,
                          const glm::vec3 &tint = glm::vec3(1.0f),
                          float useTexture = 1.0f) {
        if (!mdl) return;
        glm::mat4 M(1.0f);
        M = glm::translate(M, pos);
        if (rotYdeg != 0.0f) M = glm::rotate(M, glm::radians(rotYdeg), glm::vec3(0, 1, 0));
        M = glm::scale(M, scale);
        m_basicShader->set_mat4("model", M);

        m_basicShader->set_vec3("uColorTint", tint);
        m_basicShader->set_float("uUseTexture", useTexture);

        mdl->draw(m_basicShader);
    };


    draw_model(m_tree, {3.0f, -0.2f, -12.0f}, {0.7f, 0.7f, 0.7f}, 0.0f, {0.85f, 1.0f, 0.85f}, 1.0f);


    draw_model(m_campfire, {0.0f, -1.2f, -8.0f}, {0.02f, 0.02f, 0.02f}, 0.0f, {1.00f, 0.85f, 0.60f}, 1.0f);

    draw_model(m_tent, {-5.0f, -0.3f, -13.0f}, {0.9f, 0.9f, 0.9f}, 0.0f, {0.95f, 0.90f, 0.80f}, 1.0f);


    draw_model(m_stone, {-2.0f, -0.2f, -9.0f}, {0.7f, 0.7f, 0.5f}, 0.0f, {0.80f, 0.85f, 0.90f}, 1.0f);


    draw_model(m_lantern, {1.2f, 0.0f, -7.8f}, {0.02f, 0.02f, 0.02f}, 20.0f, {1.00f, 0.90f, 0.60f}, 0.0f);
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

}// namespace app
