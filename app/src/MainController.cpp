#include "MainController.hpp"

#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/Mesh.hpp>

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
    m_campfire = resources->model("campfire");
    m_tent = resources->model("tent");
    m_stone = resources->model("stone");
    m_lantern = resources->model("lantern");
    m_deadtrunk = resources->model("deadtrunk");

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    graphics->perspective_params().FOV = glm::radians(100.0f);
    camera->Position = glm::vec3(0.0f, 2.5f, 3.0f);
    camera->Front = glm::normalize(glm::vec3(0.0f, -0.3f, -1.0f));

    if (m_basicShader) {
        m_basicShader->use();
        // jači, hladniji "mesec"
        m_basicShader->set_vec3("uDirLightDir", glm::normalize(glm::vec3(-0.18f, -1.0f, -0.08f)));
        m_basicShader->set_vec3("uDirLightColor", glm::vec3(0.50f, 0.55f, 0.75f));

        // materijal (malo svetlije okruženje/trava)
        m_basicShader->set_float("uAmbient", 0.34f);
        m_basicShader->set_float("uSpecularStrength", 0.25f);
        m_basicShader->set_float("uShininess", 32.0f);

        // fake bloom
        m_basicShader->set_float("uExposure", 1.40f);
        m_basicShader->set_float("uBloomFakeStrength", 1.85f);
        m_basicShader->set_float("uGlowFalloffFire", 1.35f);
        m_basicShader->set_float("uGlowFalloffLamp", 2.0f);
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

    // WASD + strelice
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

    // Skybox
    if (m_skyboxShader && m_skybox) graphics->draw_skybox(m_skyboxShader, m_skybox);
    if (!m_basicShader) return;

    m_basicShader->use();
    m_basicShader->set_mat4("projection", graphics->projection_matrix());
    m_basicShader->set_mat4("view", graphics->camera()->view_matrix());
    m_basicShader->set_vec3("uViewPos", graphics->camera()->Position);

    // "tlo" = zajednička visina svih modela (deluje kao da stoje na dnu skyboxa)
    const float groundY = -0.34f;
    const glm::vec3 campCenter = {0.0f, groundY, -7.0f};

    // vatra u centru (flicker)
    static float t = 0.0f;
    t += platform->dt();
    const float k = 0.6f + 0.4f * std::sin(t * 5.0f);
    const glm::vec3 fireA(1.00f, 0.38f, 0.06f);
    const glm::vec3 fireB(1.00f, 0.82f, 0.36f);
    const glm::vec3 fireCol = (1.0f - k) * fireA + k * fireB;

    const glm::vec3 firePos = campCenter + glm::vec3(0.0f, 1.25f, 0.0f);
    m_basicShader->set_vec3("uLightPos", firePos);
    m_basicShader->set_vec3("uLightColor", fireCol);

    // fenjer — VEĆI i USPRAVAN
    static bool prevL = false;
    static float lanternOn = 1.0f;
    const bool currL = platform->key(engine::platform::KeyId::KEY_L).is_down();
    if (currL && !prevL) lanternOn = (lanternOn > 0.5f) ? 0.0f : 1.0f;
    prevL = currL;

    const glm::vec3 lanternPos = campCenter + glm::vec3(3.2f, groundY + 0.35f, 2.8f);
    m_basicShader->set_vec3("uLanternPos", lanternPos);
    m_basicShader->set_vec3("uLanternColor", glm::vec3(1.0f, 0.90f, 0.56f));
    m_basicShader->set_float("uLanternOn", lanternOn);

    // helper za single-draw
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

    // --- single modeli ---
    draw_model(m_campfire, campCenter + glm::vec3(0.0f, -0.02f, 0.0f),
               glm::vec3(0.28f), glm::vec3(0.0f, 10.0f, 0.0f),
               glm::vec3(1.0f, 0.95f, 0.88f), 1.0f);

    // fenjer uspravno (X rot = 0), malo veći
    draw_model(m_lantern, lanternPos,
               glm::vec3(0.04f),
               glm::vec3(0.0f, -22.0f, 0.0f),
               glm::vec3(1.0f, 0.98f, 0.86f), 0.0f);

    // kamen veći i odmaknut
    draw_model(m_stone, campCenter + glm::vec3(-5.0f, -0.03f, 3.0f),
               glm::vec3(1.15f), glm::vec3(0.0f, 18.0f, 0.0f),
               glm::vec3(0.94f, 0.96f, 0.98f), 1.0f);

    // šator veći i dalje od pentagona
    draw_model(m_tent, campCenter + glm::vec3(8.5f, -0.02f, -13.5f),
               glm::vec3(2.10f), glm::vec3(0.0f, 28.0f, 0.0f),
               glm::vec3(0.95f, 0.90f, 0.82f), 1.0f);

    // jedno drvo (primerak)
    draw_model(m_tree, campCenter + glm::vec3(-10.0f, 0.0f, -18.0f),
               glm::vec3(1.25f), glm::vec3(0.0f, -12.0f, 0.0f),
               glm::vec3(0.90f, 1.0f, 0.90f), 1.0f);

    // jedno deblo (van pentagona, čisto primerak)
    draw_model(m_deadtrunk, campCenter + glm::vec3(-6.5f, -0.03f, -3.4f),
               glm::vec3(0.46f), glm::vec3(0.0f, 24.0f, 10.0f));

    // ===================== GPU INSTANCING =====================
    static bool instancingInit = false;
    static std::vector<glm::mat4> TRUNK_MATS;// panjevi oko vatre (pentagon)
    static std::vector<glm::mat4> TREE_MATS; // raznovrsna stabla
    static std::vector<uint32_t> TRUNK_VBOS;
    static std::vector<uint32_t> TREE_VBOS;

    if (!instancingInit) {
        instancingInit = true;

        auto frand = [](float a, float b) {
            static uint32_t s = 0x9E3779B9u;
            s ^= s << 13;
            s ^= s >> 17;
            s ^= s << 5;
            float t = (s & 0xFFFFFF) / float(0xFFFFFF);
            return a + (b - a) * t;
        };

        // ---- 1) panjevi kao PENTAGON oko logorske vatre ----
        const int N_LOGS = 5;        // 4 ili 5; stavili smo 5 (pravilni petougao)
        const float R_LOGS = 2.35f;  // radijus kruga na kome su “klupe”
        const float LOG_MINS = 0.42f;// skala panja
        const float LOG_MAXS = 0.55f;

        TRUNK_MATS.reserve(N_LOGS);
        for (int i = 0; i < N_LOGS; ++i) {
            float ang = glm::two_pi<float>() * (float) i / (float) N_LOGS;
            glm::vec3 p = campCenter + glm::vec3(std::cos(ang) * R_LOGS, -0.03f, std::sin(ang) * R_LOGS);

            // neka “klupa” (deblo) gleda ka centru
            float yawDeg = glm::degrees(atan2f(campCenter.z - p.z, campCenter.x - p.x));
            float rollDeg = frand(-7.0f, 7.0f);// malo nagni

            float s = frand(LOG_MINS, LOG_MAXS);

            glm::mat4 M(1.0f);
            M = glm::translate(M, p);
            M = glm::rotate(M, glm::radians(yawDeg), glm::vec3(0, 1, 0)); // okreni ka vatri
            M = glm::rotate(M, glm::radians(rollDeg), glm::vec3(0, 0, 1));// malo random nagni
            M = glm::scale(M, glm::vec3(s));
            TRUNK_MATS.push_back(M);
        }

        // ---- 2) stabla – više raznovrsnosti u veličini i rasporedu ----
        const int N_TREES = 14;
        const float R_MIN_TREE = 12.0f, R_MAX_TREE = 22.0f;// širi krug
        const float TREE_MIN_S = 0.90f, TREE_MAX_S = 1.85f;// veći raspon skala

        TREE_MATS.reserve(N_TREES);
        for (int i = 0; i < N_TREES; ++i) {
            float ang = glm::two_pi<float>() * (float) i / (float) N_TREES + frand(-0.2f, 0.2f);
            float r = frand(R_MIN_TREE, R_MAX_TREE);
            glm::vec3 p = campCenter + glm::vec3(std::cos(ang) * r, 0.0f, std::sin(ang) * r);

            float yawDeg = frand(-30.0f, 30.0f);
            float s = frand(TREE_MIN_S, TREE_MAX_S);

            glm::mat4 M(1.0f);
            M = glm::translate(M, p);
            M = glm::rotate(M, glm::radians(yawDeg), glm::vec3(0, 1, 0));
            M = glm::scale(M, glm::vec3(s));
            TREE_MATS.push_back(M);
        }

        // upload + kačenje instance matrica (attrib base = 8)
        TRUNK_VBOS.reserve(m_deadtrunk->meshes().size());
        for (auto &msh: m_deadtrunk->meshes()) TRUNK_VBOS.push_back(msh.attach_instance_matrices(TRUNK_MATS, 8));

        TREE_VBOS.reserve(m_tree->meshes().size());
        for (auto &msh: m_tree->meshes()) TREE_VBOS.push_back(msh.attach_instance_matrices(TREE_MATS, 8));
    }

    // crtanje instanci (teksture se binduju unutar Mesh::draw_instanced)
    m_basicShader->set_bool("uInstanced", true);
    for (auto &msh: m_deadtrunk->meshes()) msh.draw_instanced(m_basicShader, (uint32_t) TRUNK_MATS.size());
    for (auto &msh: m_tree->meshes()) msh.draw_instanced(m_basicShader, (uint32_t) TREE_MATS.size());
    m_basicShader->set_bool("uInstanced", false);
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

}// namespace app
