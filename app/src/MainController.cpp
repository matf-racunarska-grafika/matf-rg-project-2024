#include "MainController.hpp"

#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/graphics/GraphicsController.hpp>

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

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

    // Shaders & skybox
    m_skyboxShader = resources->shader("skybox");
    m_basicShader = resources->shader("basic");
    m_skybox = resources->skybox("dikhololo_night");

    // Models
    m_tree = resources->model("tree");
    m_campfire = resources->model("campfire");// novi obj (već sadrži svoje klade)
    m_tent = resources->model("tent");
    m_stone = resources->model("stone");
    m_lantern = resources->model("lantern");
    m_deadtrunk = resources->model("deadtrunk");// JEDAN nezavisan trupac

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    graphics->perspective_params().FOV = glm::radians(100.0f);
    camera->Position = glm::vec3(0.0f, 2.5f, 3.0f);
    camera->Front = glm::normalize(glm::vec3(0.0f, -0.3f, -1.0f));

    if (m_basicShader) {
        m_basicShader->use();
        m_basicShader->set_vec3("uDirLightDir", glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)));
        m_basicShader->set_vec3("uDirLightColor", glm::vec3(0.25f, 0.30f, 0.45f));
        m_basicShader->set_float("uAmbient", 0.15f);
        m_basicShader->set_float("uSpecularStrength", 0.25f);
        m_basicShader->set_float("uShininess", 32.0f);
    }
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

    bool sprint = platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).is_down()
                  || platform->key(engine::platform::KeyId::KEY_RIGHT_SHIFT).is_down();
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
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (m_skyboxShader && m_skybox) graphics->draw_skybox(m_skyboxShader, m_skybox);
    if (!m_basicShader) return;

    m_basicShader->use();
    m_basicShader->set_mat4("projection", graphics->projection_matrix());
    m_basicShader->set_mat4("view", graphics->camera()->view_matrix());
    m_basicShader->set_vec3("uViewPos", graphics->camera()->Position);

    const glm::vec3 campCenter = {0.0f, -1.0f, -7.0f};
    glm::vec3 firePos = campCenter + glm::vec3(0.0f, 1.3f, 0.0f);

    static float t = 0.0f;
    t += platform->dt();
    float k = 0.6f + 0.4f * std::sin(t * 5.0f);
    glm::vec3 fireA(0.95f, 0.40f, 0.08f);
    glm::vec3 fireB(1.00f, 0.80f, 0.35f);
    glm::vec3 fireCol = (1.0f - k) * fireA + k * fireB;

    m_basicShader->set_vec3("uLightPos", firePos);
    m_basicShader->set_vec3("uLightColor", fireCol);

    // Lantern toggle (L)
    static bool prevL = false;
    static float lanternOn = 1.0f;
    bool currL = platform->key(engine::platform::KeyId::KEY_L).is_down();
    if (currL && !prevL) lanternOn = (lanternOn > 0.5f) ? 0.0f : 1.0f;
    prevL = currL;

    const glm::vec3 lanternPos = campCenter + glm::vec3(1.8f, 0.20f, 0.8f);
    m_basicShader->set_vec3("uLanternPos", lanternPos);
    m_basicShader->set_vec3("uLanternColor", glm::vec3(1.0f, 0.85f, 0.50f));
    m_basicShader->set_float("uLanternOn", lanternOn);

    // helper
    auto draw_model = [&](engine::resources::Model *mdl,
                          const glm::vec3 &pos,
                          const glm::vec3 &scale = glm::vec3(1.0f),
                          const glm::vec3 &rotEulerDeg = glm::vec3(0.0f),
                          const glm::vec3 &tint = glm::vec3(1.0f),
                          float useTexture = 1.0f) {
        if (!mdl) return;
        glm::mat4 M(1.0f);
        M = glm::translate(M, pos);
        if (rotEulerDeg.x != 0.0f) M = glm::rotate(M, glm::radians(rotEulerDeg.x), glm::vec3(1, 0, 0));
        if (rotEulerDeg.y != 0.0f) M = glm::rotate(M, glm::radians(rotEulerDeg.y), glm::vec3(0, 1, 0));
        if (rotEulerDeg.z != 0.0f) M = glm::rotate(M, glm::radians(rotEulerDeg.z), glm::vec3(0, 0, 1));
        M = glm::scale(M, scale);
        m_basicShader->set_mat4("model", M);
        m_basicShader->set_vec3("uColorTint", tint);
        m_basicShader->set_float("uUseTexture", useTexture);
        mdl->draw(m_basicShader);
    };


    draw_model(m_campfire, campCenter + glm::vec3(0.0f, -0.05f, 0.0f),
               {0.18f, 0.18f, 0.18f}, {0.0f, 10.0f, 0.0f}, {1.0f, 0.92f, 0.82f}, 1.0f);


    if (m_stone) {
        const int N = 8;
        const float R = 1.2f;
        const float y = -0.15f;
        for (int i = 0; i < N; ++i) {
            float a = (2.0f * 3.1415926f * i) / N;
            glm::vec3 p = campCenter + glm::vec3(R * std::cos(a), y, R * std::sin(a));
            float yaw = glm::degrees(a) + 12.0f;// malo „random”
            draw_model(m_stone, p, {0.55f, 0.55f, 0.55f}, {0.0f, yaw, 0.0f},
                       {0.90f, 0.92f, 0.95f}, 1.0f);
        }
    }

    draw_model(m_lantern, lanternPos, {0.02f, 0.02f, 0.02f},
               {-90.0f, -30.0f, 0.0f}, {1.0f, 0.95f, 0.80f}, 0.0f);

    draw_model(m_deadtrunk, campCenter + glm::vec3(-2.6f, -0.20f, -0.5f),
               {0.60f, 0.60f, 0.60f}, {0.0f, 25.0f, 12.0f},
               {1.0f, 1.0f, 1.0f}, 1.0f);

    draw_model(m_tent, campCenter + glm::vec3(-5.0f, -0.30f, -6.0f),
               {0.95f, 0.95f, 0.95f}, {-90.0f, 15.0f, 0.0f},
               {0.95f, 0.90f, 0.82f}, 1.0f);

    draw_model(m_tree, campCenter + glm::vec3(5.0f, -0.20f, -6.5f),
               {0.75f, 0.75f, 0.75f}, {0.0f, -10.0f, 0.0f},
               {0.90f, 1.0f, 0.90f}, 1.0f);
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

}// namespace app
