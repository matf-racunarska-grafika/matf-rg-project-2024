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

static float CAM_ZOOM_DEG = 64.0f;
static glm::vec3 CAM_POS = {0.0f, 7.0f, 30.0f};

static const float GROUND_Y = -0.34f;
static glm::vec3 CAMP_CENTER = {0.0f, GROUND_Y, -7.0f};

static float CAMPFIRE_SCALE = 0.66f;
static float STONE_RADIUS = 6.5f;
static float STONE_ANGLE_DEG = 50.0f;
static float LANTERN_HEIGHT = 1.75f;
static float TENT_RADIUS = 9.5f;
static float TENT_ANGLE_DEG = 180.0f;
static glm::vec3 TENT_SCALE = {1.5f, 1.5f, 1.5f};

static glm::vec3 TENT_OFFSET = {-2.50f, -0.06f, +0.80f};

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
    camera->Zoom = CAM_ZOOM_DEG;
    graphics->perspective_params().FOV = glm::radians(camera->Zoom);

    camera->Position = CAM_POS;
    const glm::vec3 lookTarget = CAMP_CENTER + glm::vec3(0.0f, 1.8f, 0.0f);
    camera->Front = glm::normalize(lookTarget - camera->Position);

    if (m_basicShader) {
        m_basicShader->use();

        m_basicShader->set_vec3("uDirLightDir", glm::normalize(glm::vec3(-0.18f, -1.0f, -0.08f)));
        m_basicShader->set_vec3("uDirLightColor", glm::vec3(0.46f, 0.50f, 0.70f));
        m_basicShader->set_float("uAmbient", 0.18f);
        m_basicShader->set_float("uSpecularStrength", 0.22f);
        m_basicShader->set_float("uShininess", 24.0f);
        m_basicShader->set_float("uExposure", 1.05f);
        m_basicShader->set_float("uBloomFakeStrength", 1.30f);
        m_basicShader->set_float("uGlowFalloffFire", 1.45f);
        m_basicShader->set_float("uGlowFalloffLamp", 1.95f);
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
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    if (m_skyboxShader && m_skybox) graphics->draw_skybox(m_skyboxShader, m_skybox);
    if (!m_basicShader) return;

    m_basicShader->use();
    m_basicShader->set_mat4("projection", graphics->projection_matrix());
    m_basicShader->set_mat4("view", graphics->camera()->view_matrix());
    m_basicShader->set_vec3("uViewPos", graphics->camera()->Position);

    // ground
    static engine::resources::Model *s_ground = resources->model("ground");
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


    if (s_ground) {
        m_basicShader->set_float("uUvScale", 6.0f);
        draw_model(s_ground,
                   CAMP_CENTER + glm::vec3(0.0f, -0.005f, 0.0f),
                   glm::vec3(120.0f, 1.0f, 120.0f),
                   glm::vec3(0.0f),
                   glm::vec3(0.70f, 0.75f, 0.70f),
                   1.0f);
        m_basicShader->set_float("uUvScale", 1.0f);
    }


    static float t = 0.0f;
    t += platform->dt();
    float k = 0.6f + 0.4f * std::sin(t * 5.2f);
    glm::vec3 fireA = glm::vec3(1.00f, 0.38f, 0.06f);
    glm::vec3 fireB = glm::vec3(1.00f, 0.82f, 0.36f);
    glm::vec3 fireCol = (1.0f - k) * fireA + k * fireB;
    glm::vec3 firePos = CAMP_CENTER + glm::vec3(0.0f, 1.25f, 0.0f);
    m_basicShader->set_vec3("uLightPos", firePos);
    m_basicShader->set_vec3("uLightColor", fireCol * 1.25f);

    // fenjer toggle (L)
    static bool prevL = false;
    static float lanternOn = 1.0f;
    bool currL = platform->key(engine::platform::KeyId::KEY_L).is_down();
    if (currL && !prevL) lanternOn = (lanternOn > 0.5f) ? 0.0f : 1.0f;
    prevL = currL;

    draw_model(m_campfire, CAMP_CENTER + glm::vec3(0.0f, -0.02f, 0.0f),
               glm::vec3(CAMPFIRE_SCALE), glm::vec3(0.0f, 10.0f, 0.0f),
               glm::vec3(1.0f, 0.95f, 0.88f), 1.0f);

    const float angStone = glm::radians(STONE_ANGLE_DEG);
    const float angTent = glm::radians(TENT_ANGLE_DEG);

    glm::vec3 stonePos = CAMP_CENTER + glm::vec3(std::cos(angStone) * STONE_RADIUS, -0.03f,
                                                 std::sin(angStone) * STONE_RADIUS);


    glm::vec3 tentPos = CAMP_CENTER + glm::vec3(std::cos(angTent) * TENT_RADIUS, -0.02f,
                                                std::sin(angTent) * TENT_RADIUS);
    tentPos += TENT_OFFSET;


    draw_model(m_stone, stonePos,
               glm::vec3(1.25f), glm::vec3(0.0f, 12.0f, 0.0f),
               glm::vec3(0.7f, 0.75f, 0.8f),
               0.0f);


    glm::vec3 lanternPos = stonePos + glm::vec3(0.0f, LANTERN_HEIGHT, 0.0f);
    m_basicShader->set_vec3("uLanternPos", lanternPos);
    m_basicShader->set_vec3("uLanternColor", glm::vec3(1.0f, 0.90f, 0.56f));
    m_basicShader->set_float("uLanternOn", lanternOn);
    draw_model(m_lantern, lanternPos,
               glm::vec3(0.05f), glm::vec3(-90.0f, 0.0f, 0.0f),
               glm::vec3(1.0f, 0.98f, 0.86f), 0.0f);

    // šator - USE SOLID COLOR (BEIGE/BROWN)
    draw_model(m_tent, tentPos,
               TENT_SCALE, glm::vec3(0.0f, -30.0f, 0.0f),
               glm::vec3(0.8f, 0.75f, 0.6f),
               0.0f);


    static bool instancingInit = false;
    static std::vector<glm::mat4> TRUNK_MATS;
    static std::vector<glm::mat4> TREE_MATS;
    static std::vector<uint32_t> TRUNK_VBOS;
    static std::vector<uint32_t> TREE_VBOS;

    if (!instancingInit) {
        instancingInit = true;

        TRUNK_MATS.clear();

        static const float LOG_HALF = 5.0f;
        static const float LOG_SCALE = 0.30f;


        const glm::vec3 squareSides[4] = {
                CAMP_CENTER + glm::vec3(0.0f, -0.03f, -LOG_HALF),
                CAMP_CENTER + glm::vec3(LOG_HALF, -0.03f, 0.0f),
                CAMP_CENTER + glm::vec3(0.0f, -0.03f, LOG_HALF),
                CAMP_CENTER + glm::vec3(-LOG_HALF, -0.03f, 0.0f)
        };

        const float rotations[4] = {
                0.0f,
                90.0f,
                0.0f,
                90.0f
        };

        for (int i = 0; i < 4; ++i) {
            glm::mat4 M(1.0f);
            M = glm::translate(M, squareSides[i]);
            M = glm::rotate(M, glm::radians(rotations[i]), glm::vec3(0, 1, 0));
            M = glm::scale(M, glm::vec3(LOG_SCALE));
            TRUNK_MATS.push_back(M);
        }

        auto frand = [](float a, float b) {
            static uint32_t s = 0x9E3779B9u;
            s ^= s << 13;
            s ^= s >> 17;
            s ^= s << 5;
            float t = (s & 0xFFFFFF) / float(0xFFFFFF);
            return a + (b - a) * t;
        };

        const int N_TREES = 14;
        const float R_MIN_TREE = 20.0f, R_MAX_TREE = 34.0f;
        const float TREE_MIN_S = 0.6f, TREE_MAX_S = 1.2f;

        TREE_MATS.reserve(N_TREES);
        for (int i = 0; i < N_TREES; ++i) {
            float ang = glm::two_pi<float>() * (float) i / (float) N_TREES + frand(-0.22f, 0.22f);
            float r = frand(R_MIN_TREE, R_MAX_TREE);
            glm::vec3 p = CAMP_CENTER + glm::vec3(std::cos(ang) * r, 0.0f, std::sin(ang) * r);

            float yawDeg = frand(-30.0f, 30.0f);
            float s = frand(TREE_MIN_S, TREE_MAX_S);

            glm::mat4 M(1.0f);
            M = glm::translate(M, p);
            M = glm::rotate(M, glm::radians(yawDeg), glm::vec3(0, 1, 0));
            M = glm::scale(M, glm::vec3(s));
            TREE_MATS.push_back(M);
        }

        TRUNK_VBOS.reserve(m_deadtrunk->meshes().size());
        for (auto &msh: m_deadtrunk->meshes()) TRUNK_VBOS.push_back(msh.attach_instance_matrices(TRUNK_MATS, 8));

        TREE_VBOS.reserve(m_tree->meshes().size());
        for (auto &msh: m_tree->meshes()) TREE_VBOS.push_back(msh.attach_instance_matrices(TREE_MATS, 8));
    }


    m_basicShader->set_bool("uInstanced", true);


    m_basicShader->set_float("uUseTexture", 0.0f);
    m_basicShader->set_vec3("uColorTint", glm::vec3(0.4f, 0.6f, 0.3f));
    for (auto &msh: m_tree->meshes()) msh.draw_instanced(m_basicShader, (uint32_t) TREE_MATS.size());


    m_basicShader->set_float("uUseTexture", 1.0f);
    m_basicShader->set_vec3("uColorTint", glm::vec3(1.0f));
    for (auto &msh: m_deadtrunk->meshes()) msh.draw_instanced(m_basicShader, (uint32_t) TRUNK_MATS.size());

    m_basicShader->set_bool("uInstanced", false);
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

}// namespace app