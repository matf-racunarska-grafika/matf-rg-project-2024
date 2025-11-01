#include "MainController.h"
#include "GUIController.hpp"
#include "engine/graphics/BloomController.h"
#include "engine/graphics/SpriteController.hpp"
#include "engine/core/Controller.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"
#include "spdlog/spdlog.h"

namespace app {
static engine::graphics::BloomController* g_bloom = nullptr;
static bool g_b_pressed = false;
static engine::resources::Texture* g_star_tex = nullptr;

// Klasa za praćenje miša
class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx, position.dy);
}

// ----------------------
void MainController::initialize() {
    spdlog::info("MainController initialized");

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());

    engine::graphics::OpenGL::enable_depth_testing();

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto cam = graphics->camera();
    cam->Position = glm::vec3(-33.941f, 18.9701f, -10.2835f);
    cam->Yaw = 22.06f;
    cam->Pitch = 13.39f;
    cam->rotate_camera(0.0f, 0.0f);
    cam->Position -= cam->Front * 30.0f;

    // Inicijalizuj skybox shader (postavi sampler)
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skyboxShader = resources->shader("skybox");
    skyboxShader->use();
    skyboxShader->set_int("skybox", 0);

    // Bloom setup
    g_bloom = engine::core::Controller::get<engine::graphics::BloomController>();
    if (g_bloom) {
        g_bloom->bloom_setup();
        g_bloom->toggle_bloom();
    }

    // Učitaj star teksturu
    if (!g_star_tex) {
        g_star_tex = resources->texture("star", "resources/textures/star.png");
    }
}

// ----------------------
bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        spdlog::info("Escape pressed — exiting app.");
        return false;
    }

    // Toggle bloom na B
    if (g_bloom) {
        if (platform->key(engine::platform::KeyId::KEY_B).is_down()) {
            if (!g_b_pressed) {
                g_bloom->toggle_bloom();
                spdlog::info("ACTION_B: bloom toggled -> {}", g_bloom->is_bloom_enabled() ? "ON" : "OFF");
                g_b_pressed = true;
            }
        } else {
            g_b_pressed = false;
        }
    }

    // Direktni tasteri za EVENT_A i EVENT_C radi lakšeg testiranja
    if (platform->key(engine::platform::KeyId::KEY_A)
                .state() == engine::platform::Key::State::JustPressed) {
        // Postavi svetlo na roze nijansu i pojačaj intenzitet
        const glm::vec3 pink = glm::vec3(1.2f, 0.3f, 0.9f);
        m_point_light.diffuse  = pink * 3.0f;
        m_point_light.specular = pink * 3.0f;
        m_point_light.ambient  = pink * 0.5f; // i ambient ide u roze, ali slabije
        m_event_a_done = true;
        spdlog::info("EVENT_A: point light color changed");
    }
    if (platform->key(engine::platform::KeyId::KEY_C)
                .state() == engine::platform::Key::State::JustPressed) {
        m_tulip_visible = !m_tulip_visible;
        m_event_c_done = true;
        spdlog::info("EVENT_C: tulip visible = {}", m_tulip_visible);
    }

    // ACTION: pokreni event sekvencu tasterom SPACE
    if (platform->key(engine::platform::KeyId::KEY_SPACE)
                .state() == engine::platform::Key::State::JustPressed) {
        m_sequence_running = true;
        m_sequence_time = 0.0f;
        m_event_a_done = m_event_b_done = m_event_c_done = false;
    }
    return true;
}

// ----------------------
void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

// Crtanje meseca
void MainController::draw_moon() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    engine::resources::Model* moon = resources->model("moon");
    auto shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("viewPos", graphics->camera()->Position);
    shader->set_float("shininess", m_shininess);
    // Directional light
    shader->set_vec3("dirLight.direction", m_dir_light.direction);
    shader->set_vec3("dirLight.ambient",   m_dir_light.ambient);
    shader->set_vec3("dirLight.diffuse",   m_dir_light.diffuse);
    shader->set_vec3("dirLight.specular",  m_dir_light.specular);
    // Point light
    shader->set_vec3("pointLight.position",  m_point_light.position);
    shader->set_float("pointLight.constant",  m_point_light.constant);
    shader->set_float("pointLight.linear",    m_point_light.linear);
    shader->set_float("pointLight.quadratic", m_point_light.quadratic);
    shader->set_vec3("pointLight.ambient",    m_point_light.ambient);
    shader->set_vec3("pointLight.diffuse",    m_point_light.diffuse);
    shader->set_vec3("pointLight.specular",   m_point_light.specular);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(8.0f, 8.0f, 8.0f) + m_moon_event_offset);
    model = glm::scale(model, glm::vec3(8.25f));

    shader->set_mat4("model", model);
    moon->draw(shader);
}

// Crtanje tulipa
void MainController::draw_tulip() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    if (!m_tulip_visible) return;
    engine::resources::Model* tulip = resources->model("tulip");
    auto shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("viewPos", graphics->camera()->Position);
    shader->set_float("shininess", m_shininess);
    shader->set_vec3("dirLight.direction", m_dir_light.direction);
    shader->set_vec3("dirLight.ambient",   m_dir_light.ambient);
    shader->set_vec3("dirLight.diffuse",   m_dir_light.diffuse);
    shader->set_vec3("dirLight.specular",  m_dir_light.specular);
    shader->set_vec3("pointLight.position",  m_point_light.position);
    shader->set_float("pointLight.constant",  m_point_light.constant);
    shader->set_float("pointLight.linear",    m_point_light.linear);
    shader->set_float("pointLight.quadratic", m_point_light.quadratic);
    shader->set_vec3("pointLight.ambient",    m_point_light.ambient);
    shader->set_vec3("pointLight.diffuse",    m_point_light.diffuse);
    shader->set_vec3("pointLight.specular",   m_point_light.specular);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 19.0f, 9.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.37f));

    shader->set_mat4("model", model);
    tulip->draw(shader);
}

// Glavna draw funkcija
void MainController::draw() {
    if (g_bloom && g_bloom->is_bloom_enabled()) {
        g_bloom->prepare_hdr();
    }
    
    draw_moon();
    draw_tulip();

    // Nacrtaj skybox u pozadini
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    {
        std::filesystem::path sb = "resources/skyboxes/skybox";
        if (!std::filesystem::is_directory(sb)) {
            sb = "app/resources/skyboxes/skybox";
        }
        graphics->draw_skybox(resources->shader("skybox"), resources->skybox("skybox", sb));
    }

    if (g_bloom && g_bloom->is_bloom_enabled()) {
        g_bloom->finalize_bloom();
    }

    {
        auto shader = resources->shader("blending");

        glm::mat4 modelStar = glm::mat4(1.0f);
        modelStar = glm::translate(modelStar, glm::vec3(23.0f, 0.0f, 5.0f));
        modelStar = glm::rotate(modelStar, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 1.0f));

        modelStar = glm::scale(modelStar, glm::vec3(90.0f));

        engine::core::Controller::get<engine::graphics::SpriteController>()->
            draw_textured_quad(g_star_tex, shader, modelStar,
                               graphics->camera()->view_matrix(), graphics->projection_matrix());
    }
}

// ----------------------
void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

// ----------------------
void MainController::update_camera() {
    auto gui = engine::core::Controller::get<app::GUIController>();
    if (gui && gui->is_enabled()) {
        return;
    }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KEY_W).state() == engine::platform::Key::State::Pressed)
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    if (platform->key(engine::platform::KEY_S).state() == engine::platform::Key::State::Pressed)
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    if (platform->key(engine::platform::KEY_A).state() == engine::platform::Key::State::Pressed)
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    if (platform->key(engine::platform::KEY_D).state() == engine::platform::Key::State::Pressed)
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);

    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

void MainController::update() {
    update_camera();

    // Event sekvenca – vreme i okidači
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (m_sequence_running) {
        m_sequence_time += platform->dt();
        // EVENT_A: nakon 5s – promeni boju point svetla (npr. roze)
        if (!m_event_a_done && m_sequence_time >= 5.0f) {
            m_point_light.diffuse = glm::vec3(1.0f, 0.3f, 0.8f);
            m_event_a_done = true;
        }
        // EVENT_B: nakon 8s – pomeri mesec
        if (!m_event_b_done && m_sequence_time >= 8.0f) {
            m_moon_event_offset = glm::vec3(4.0f, 2.0f, 0.0f);
            m_event_b_done = true;
            spdlog::info("EVENT_B: moon moved by offset (4,2,0)");
        }
        // EVENT_C: nakon 12s – sakrij tulip
        if (!m_event_c_done && m_sequence_time >= 12.0f) {
            m_tulip_visible = false;
            m_event_c_done = true;
        }
        // reset posle 18s
        if (m_sequence_time >= 18.0f) {
            m_sequence_running = false;
            m_sequence_time = 0.0f;
            m_moon_event_offset = glm::vec3(0.0f);
            m_tulip_visible = true;
            // vrati svetlo
            m_point_light.diffuse = glm::vec3(0.8f);
        }
    }
}

} // namespace app
