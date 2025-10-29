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
static glm::vec3 CAM_POS = {0.0f, 7.0f, 25.0f};

static const float GROUND_Y = -0.34f;
static glm::vec3 CAMP_CENTER = {0.0f, GROUND_Y, -7.0f};

static float CAMPFIRE_SCALE = 0.66f;
static float STONE_RADIUS = 5.5f;
static float STONE_ANGLE_DEG = 28.0f;
static float LANTERN_HEIGHT = 1.75f;
static float TENT_RADIUS = 8.4f;
static float TENT_ANGLE_DEG = 180.0f;
static glm::vec3 TENT_SCALE = {1.8f, 1.8f, 1.8f};
static glm::vec3 TENT_OFFSET = {-2.50f, -0.06f, +0.80f};

static float LOG_HALF = 5.0f;
static float LOG_SCALE = 0.30f;

static float campfireIntensity = 1.25f;
static float moonIntensity = 1.0f;
static bool spotlightEnabled = false;
static glm::vec3 spotlightColor = glm::vec3(1.0f, 1.0f, 0.8f);
static float spotlightIntensity = 2.0f;

static bool sequenceTriggered = false;
static double sequenceStartTime = 0.0;
static bool eventA_triggered = false;
static bool eventB_triggered = false;
static glm::vec3 originalTentColor = glm::vec3(0.8f, 0.75f, 0.6f);
static glm::vec3 eventTentColor = glm::vec3(1.0f, 0.3f, 0.3f);

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
        m_basicShader->set_bool("uSpotlightEnabled", false);
        m_basicShader->set_vec3("uSpotlightColor", glm::vec3(0.0f));
        m_basicShader->set_float("uSpotlightCutoff", glm::cos(glm::radians(12.5f)));
        m_basicShader->set_float("uSpotlightOuterCutoff", glm::cos(glm::radians(17.5f)));
    }
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) return false;
    return true;
}

void MainController::update() {
    update_camera();
    update_lighting_adjustments();
    update_event_sequence();
}

void MainController::update_lighting_adjustments() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();


    if (platform->key(engine::platform::KeyId::KEY_I).is_down()) {
        campfireIntensity = std::min(3.0f, campfireIntensity + 0.05f);
        spdlog::info("Campfire intensity increased to: {:.2f}", campfireIntensity);
    }
    if (platform->key(engine::platform::KeyId::KEY_O).is_down()) {
        campfireIntensity = std::max(0.5f, campfireIntensity - 0.05f);
        spdlog::info("Campfire intensity decreased to: {:.2f}", campfireIntensity);
    }


    if (platform->key(engine::platform::KeyId::KEY_U).is_down()) {
        moonIntensity = std::min(2.0f, moonIntensity + 0.05f);
        spdlog::info("Moon intensity increased to: {:.2f}", moonIntensity);
    }
    if (platform->key(engine::platform::KeyId::KEY_J).is_down()) {
        moonIntensity = std::max(0.1f, moonIntensity - 0.05f);
        spdlog::info("Moon intensity decreased to: {:.2f}", moonIntensity);
    }


    static bool prevP = false;
    bool currP = platform->key(engine::platform::KeyId::KEY_P).is_down();
    if (currP && !prevP) {
        spotlightEnabled = !spotlightEnabled;
        spdlog::info("Spotlight {}", spotlightEnabled ? "ENABLED" : "DISABLED");
    }
    prevP = currP;


    if (platform->key(engine::platform::KeyId::KEY_M).is_down()) {
        spotlightIntensity = std::min(5.0f, spotlightIntensity + 0.1f);
        spdlog::info("Spotlight intensity increased to: {:.2f}", spotlightIntensity);
    }
    if (platform->key(engine::platform::KeyId::KEY_N).is_down()) {
        spotlightIntensity = std::max(0.5f, spotlightIntensity - 0.1f);
        spdlog::info("Spotlight intensity decreased to: {:.2f}", spotlightIntensity);
    }


    if (platform->key(engine::platform::KeyId::KEY_H).is_down()) {
        // H for help
        spdlog::info("=== LIGHTING CONTROLS ===");
        spdlog::info("Campfire: I/O (Current: {:.2f})", campfireIntensity);
        spdlog::info("Moon: U/J (Current: {:.2f})", moonIntensity);
        spdlog::info("Spotlight: P (toggle) M/N (intensity)");
        spdlog::info("Spotlight: {} (Intensity: {:.2f})",
                     spotlightEnabled ? "ON" : "OFF", spotlightIntensity);
        spdlog::info("Lantern: L (toggle)");
    }

}

void MainController::update_event_sequence() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();


    float distanceToCampfire = glm::length(camera->Position - (CAMP_CENTER + glm::vec3(0.0f, 1.25f, 0.0f)));
    bool isNearCampfire = distanceToCampfire < 8.0f;


    static bool prevF = false;
    bool currF = platform->key(engine::platform::KeyId::KEY_F).is_down();

    if (currF && !prevF && isNearCampfire && !sequenceTriggered) {
        sequenceTriggered = true;
        sequenceStartTime = platform->frame_time().current;
        eventA_triggered = false;
        eventB_triggered = false;
        spdlog::info("Event sequence triggered! Stand by for events...");
    }
    prevF = currF;


    if (sequenceTriggered) {
        double currentTime = platform->frame_time().current;
        double elapsed = currentTime - sequenceStartTime;


        if (!eventA_triggered && elapsed >= 3.0) {
            campfireIntensity = 2.5f;
            eventA_triggered = true;
            spdlog::info("EVENT A: Campfire intensity boosted!");
        }


        if (eventA_triggered && !eventB_triggered && elapsed >= 8.0) {
            eventB_triggered = true;
            spdlog::info("EVENT B: Tent color changed!");
            sequenceStartTime = currentTime;
        }


        if (eventB_triggered && (currentTime - sequenceStartTime >= 5.0)) {
            sequenceTriggered = false;
            campfireIntensity = 1.25f;
            spdlog::info("Event sequence completed and reset!");
        }
    }
}

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


    m_basicShader->set_vec3("uDirLightColor", glm::vec3(0.46f, 0.50f, 0.70f) * moonIntensity);

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

    m_basicShader->set_bool("uSpotlightEnabled", spotlightEnabled);
    if (spotlightEnabled) {
        auto camera = graphics->camera();
        m_basicShader->set_vec3("uSpotlightPos", camera->Position);
        m_basicShader->set_vec3("uSpotlightDir", camera->Front);
        m_basicShader->set_vec3("uSpotlightColor", spotlightColor * spotlightIntensity);
        m_basicShader->set_float("uSpotlightCutoff", glm::cos(glm::radians(12.5f)));
        m_basicShader->set_float("uSpotlightOuterCutoff", glm::cos(glm::radians(17.5f)));
    } else { m_basicShader->set_vec3("uSpotlightColor", glm::vec3(0.0f)); }


    static float t = 0.0f;
    t += platform->dt();
    float k = 0.6f + 0.4f * std::sin(t * 5.2f);
    glm::vec3 fireA = glm::vec3(1.00f, 0.38f, 0.06f);
    glm::vec3 fireB = glm::vec3(1.00f, 0.82f, 0.36f);
    glm::vec3 fireCol = (1.0f - k) * fireA + k * fireB;
    glm::vec3 firePos = CAMP_CENTER + glm::vec3(0.0f, 1.25f, 0.0f);
    m_basicShader->set_vec3("uLightPos", firePos);
    m_basicShader->set_vec3("uLightColor", fireCol * campfireIntensity);


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
               glm::vec3(0.7f, 0.75f, 0.8f), 0.0f);


    glm::vec3 lanternPos = stonePos + glm::vec3(0.0f, LANTERN_HEIGHT, 0.0f);
    m_basicShader->set_vec3("uLanternPos", lanternPos);
    m_basicShader->set_vec3("uLanternColor", glm::vec3(1.0f, 0.90f, 0.56f));
    m_basicShader->set_float("uLanternOn", lanternOn);


    if (spotlightEnabled) {
        glm::vec3 spotlightDir = glm::normalize(CAMP_CENTER - lanternPos);
        m_basicShader->set_vec3("uSpotlightPos", lanternPos);
        m_basicShader->set_vec3("uSpotlightDir", spotlightDir);
        m_basicShader->set_vec3("uSpotlightColor", spotlightColor * spotlightIntensity);
        m_basicShader->set_float("uSpotlightCutoff", glm::cos(glm::radians(15.0f)));// Narrow beam
        m_basicShader->set_float("uSpotlightOuterCutoff", glm::cos(glm::radians(25.0f)));
    } else { m_basicShader->set_vec3("uSpotlightColor", glm::vec3(0.0f)); }


    glm::vec3 lanternTint = spotlightEnabled ? glm::vec3(1.2f, 1.1f, 0.9f) : glm::vec3(1.0f, 0.98f, 0.86f);
    draw_model(m_lantern, lanternPos,
               glm::vec3(0.05f), glm::vec3(-90.0f, 0.0f, 0.0f),
               lanternTint, 0.0f);

    glm::vec3 tentColor = eventB_triggered ? eventTentColor : originalTentColor;
    draw_model(m_tent, tentPos,
               TENT_SCALE, glm::vec3(0.0f, -30.0f, 0.0f),
               tentColor, 0.0f);


    draw_model(m_tree, CAMP_CENTER + glm::vec3(-12.0f, 0.0f, -20.0f),
               glm::vec3(1.25f), glm::vec3(0.0f, -12.0f, 0.0f),
               glm::vec3(0.4f, 0.6f, 0.3f), 0.0f);


    static bool instancingInit = false;
    static std::vector<glm::mat4> TRUNK_MATS;
    static std::vector<glm::mat4> TREE_MATS;
    static std::vector<uint32_t> TRUNK_VBOS;
    static std::vector<uint32_t> TREE_VBOS;

    if (!instancingInit) {
        instancingInit = true;

        TRUNK_MATS.clear();

        const glm::vec3 squareSides[4] = {
                CAMP_CENTER + glm::vec3(0.0f, -0.03f, -LOG_HALF),
                CAMP_CENTER + glm::vec3(LOG_HALF, -0.03f, 0.0f),
                CAMP_CENTER + glm::vec3(0.0f, -0.03f, LOG_HALF),
                CAMP_CENTER + glm::vec3(-LOG_HALF, -0.03f, 0.0f)
        };

        const float rotations[4] = {0.0f, 90.0f, 0.0f, 90.0f};

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