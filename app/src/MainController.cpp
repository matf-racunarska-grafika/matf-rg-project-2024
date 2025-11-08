//
// Created by zesla on 10/9/25.
//

#include <GUIController.hpp>
#include <MainController.hpp>
#include <engine/core/App.hpp>
#include <engine/graphics/DrawingController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/VisionController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <spdlog/spdlog.h>

#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

bool g_lamp_is_on = true;
bool g_lamp_is_flickering = false;
bool g_start_flicker_sequence = false;
float g_flicker_timer = 0.0f;


namespace app {


class MainPlatformEventObserver : public engine::platform::PlatformEventObserver
{
public:
    void on_mouse_move(engine::platform::MousePosition position) override;

};
void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui = engine::core::Controller::get<GUIController>();
    if(!gui->is_enabled()){
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx,position.dy);
    }
}

void MainController::initialize() {
    spdlog::info("Controller initiallized!");

    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));

    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->Position = glm::vec3(-7.0f, 7.0f, 7.0f);
    auto vision = engine::core::Controller::get<engine::vision::VisionController>();
    vision->set_bloom_threshold(1.5f);
    vision->set_bloom_intensity(0.4f);
    vision->set_exposure(1.0f);
    vision->set_blur_passes(10);

    setup_clouds();

    setup_cloud_lighting();
};
bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto vision = engine::core::Controller::get<engine::vision::VisionController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    if (platform->key(engine::platform::KeyId::KEY_L).state() == engine::platform::Key::State::JustPressed) {
        if (!m_lamp_is_flickering) {
            m_lamp_is_on = !m_lamp_is_on;
            spdlog::info("Lamp has just been (manually) turned {}", m_lamp_is_on?"on!":"off!");
        }
    }
    if (platform->key(engine::platform::KeyId::KEY_B).state() == engine::platform::Key::State::JustPressed) {
        vision->toggle_bloom();
        spdlog::info("Bloom {}", vision->is_bloom_enabled()?"on!":"off!");
    }
    if (platform->key(engine::platform::KeyId::KEY_X).state() == engine::platform::Key::State::JustPressed) {
        bool morbin = !m_morbin_time;
        m_morbin_time = morbin;
        spdlog::info("Switching time...");
    }
    return true;
}

void MainController::setup_lighting(engine::resources::Shader* shader) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_vec3("viewPos", graphics->camera()->Position);


    shader->set_vec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->set_vec3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->set_vec3("dirLight.diffuse", glm::vec3(0.2f, 0.2f, 0.3f));
    shader->set_vec3("dirLight.specular", glm::vec3(0.2f, 0.2f, 0.2f));


    shader->set_vec3("pointLights[0].ambient", glm::vec3(0)); //was set initially for purposes of testing
    shader->set_vec3("pointLights[0].diffuse", glm::vec3(0)); //now practically useless
    shader->set_vec3("pointLights[0].specular", glm::vec3(0));
    shader->set_float("pointLights[0].constant", 1.0f);
    shader->set_float("pointLights[0].linear", 0.09f);
    shader->set_float("pointLights[0].quadratic", 0.032f);


    if (m_lamp_is_on) {

        if (m_morbin_time) {
            shader->set_vec3("pointLights[1].position", m_lamp_position + glm::vec3(0.0f, 0.5f, 0.0f));
            shader->set_vec3("pointLights[1].ambient", glm::vec3(0.9f, 0.35f, 0.3f));
            shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.9f, 0.3f, 0.2f));
            shader->set_vec3("pointLights[1].ambient", glm::vec3(0.8f, 0.07f, 0.08f));
            shader->set_vec3("pointLights[1].specular", glm::vec3(0.7f, 0.45f, 0.4f));

        }
        else {
            shader->set_vec3("pointLights[1].position", m_lamp_position + glm::vec3(0.0f, 0.5f, 0.0f));
            shader->set_vec3("pointLights[1].ambient", glm::vec3(0.4f, 0.35f, 0.3f));
            shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.7f, 0.6f));
            shader->set_vec3("pointLights[1].ambient", glm::vec3(0.2f, 0.18f, 0.15f));
            shader->set_vec3("pointLights[1].specular", glm::vec3(0.5f, 0.45f, 0.4f));
        }
        shader->set_float("pointLights[1].constant", 1.0f);
        shader->set_float("pointLights[1].linear", 0.22f);
        shader->set_float("pointLights[1].quadratic", 0.20f);
    } else {
        shader->set_vec3("pointLights[1].position", m_lamp_position);
        shader->set_vec3("pointLights[1].ambient", glm::vec3(0.0f));
        shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.0f));
        shader->set_vec3("pointLights[1].specular", glm::vec3(0.0f));
        shader->set_float("pointLights[1].constant", 1.0f);
        shader->set_float("pointLights[1].linear", 1.0f);
        shader->set_float("pointLights[1].quadratic", 1.0f);
    }

    shader->set_int("numPointLights", 2);
}

void MainController::setup_cloud_lighting() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Shader* cloud_shader = resources->shader("cloud");
    if (!cloud_shader) return;

    cloud_shader->use();

    setup_lighting(cloud_shader);

    cloud_shader->set_vec3("cloudColor", glm::vec3(1.0f, 1.0f, 1.0f));
    cloud_shader->set_float("material_shininess", 8.0f);
}

void MainController::draw_bed() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model* bed = resources->model("bed1");
    engine::resources::Shader* shader = resources->shader("bed");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0);
    shader->set_mat4("model", model);

    shader->set_float("material_shininess", 2.0f);
    //shader->set_vec3("viewPos", graphics->camera()->Position);
    //shader->set_vec3("lightPos", glm::vec3(0.0f, 5.0f, 2.0f));
    //shader->set_vec3("lightColor", glm::vec3(5.0f, 5.0f, 5.0f));

    setup_lighting(shader);

    bed->draw(shader);
}

void MainController::draw_cloud() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model* cloud = resources->model("cloud");
    engine::resources::Shader* shader = resources->shader("cloud");

    shader->use();

    glm::mat4 model = glm::mat4(1.0);


    shader->set_mat4("model", model);
    setup_lighting(shader);

    shader->set_vec3("viewPos", graphics->camera()->Position);
    shader->set_vec3("lightPos", glm::vec3(0.0f, 5.0f, 0.0f));
    shader->set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    cloud->draw(shader);
}

void MainController::draw_table() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model* table = resources->model("table");
    engine::resources::Shader* shader = resources->shader("table");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(7.0f, -3.0f, 7.0f));
    model = glm::scale(model, glm::vec3(0.065f));
    shader->set_mat4("model", model);

    shader->set_float("material_shininess", 32.0f);

    setup_lighting(shader);
    table->draw(shader);

}

void MainController::draw_lamp() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model* lamp = resources->model("lamp");
    engine::resources::Shader* shader = resources->shader("lamp");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0);

    model = glm::translate(model, m_lamp_position);

    model = glm::scale(model, glm::vec3(0.09f, 0.09f, 0.09f));
    shader->set_mat4("model", model);

    shader->set_bool("lampRedGlow", m_morbin_time);
    shader->set_int("lampIsOn", m_lamp_is_on ? 1 : 0);
    shader->set_vec3("viewPos", graphics->camera()->Position);
    shader->set_float("material_shininess", 64.0f);

    setup_lighting(shader);

    if (m_lamp_is_on) {
        if (m_morbin_time) {
            shader->set_vec3("texture_emission1", glm::vec3(5.0f, 0.1f, 0.1f));
            shader->set_float("emission_strength", 3.0f);
        } else {
            shader->set_vec3("emission_color", glm::vec3(3.0f, 2.5f, 1.5f));
            shader->set_float("emission_strength", 1.5f);
        }
    } else {
        shader->set_vec3("emission_color", glm::vec3(0.0f, 0.0f, 0.0f));
        shader->set_float("emission_strength", 0.0f);
    }

    shader->set_vec3("material.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->set_vec3("material.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    shader->set_vec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader->set_float("material.shininess", 32.0f);

    lamp->draw(shader);
}

void MainController::update_lamp_flicker() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();

    if (g_start_flicker_sequence) {
        m_start_flicker_sequence = true;
        g_start_flicker_sequence = false;
    }

    if (m_start_flicker_sequence && !m_lamp_is_flickering) {
        m_start_flicker_sequence = false;
        m_flicker_delay_timer = 3.0f;
        m_lamp_is_flickering = true;
        spdlog::info("Flicker sequence started - waiting 3 seconds");
    }

    if (m_lamp_is_flickering) {
        if (m_flicker_delay_timer > 0.0f) {
            m_flicker_delay_timer -= dt;

            if (m_flicker_delay_timer <= 0.0f) {
                m_flicker_timer = 3.0f;
                m_flicker_interval = 0.0f;
                spdlog::info("Starting flicker effect");
            }
        }
        else if (m_flicker_timer > 0.0f) {
            m_flicker_timer -= dt;
            g_flicker_timer = m_flicker_timer;

            m_flicker_interval -= dt;
            if (m_flicker_interval <= 0.0f) {
                m_lamp_is_on = !m_lamp_is_on;
                g_lamp_is_on = m_lamp_is_on;

                m_flicker_interval = 0.05f + (rand() % 150) / 1000.0f;
            }

            if (m_flicker_timer <= 0.0f) {
                m_lamp_is_on = (rand() % 2) == 0;  // 50/50 chance
                g_lamp_is_on = m_lamp_is_on;
                m_lamp_is_flickering = false;
                g_lamp_is_flickering = false;

                spdlog::info("Flicker effect ended - lamp is now (due to pure luck): {}", m_lamp_is_on ? "on!" : "off!");
            }
        }
    }

    g_lamp_is_flickering = m_lamp_is_flickering;
    g_lamp_is_on = m_lamp_is_on;
}

void MainController::setup_clouds() {
    auto drawing = engine::core::Controller::get<engine::drawing::DrawingController>();

    drawing->create_instanced_group("cloud", "cloud", "cloud");

    for (int i = 0; i < m_num_clouds; i++) {
        float angle = (i / (float)m_num_clouds) * 2.0f * M_PI;
        float x = cos(angle) * m_orbit_radius;
        float z = sin(angle) * m_orbit_radius;
        float y = sin(angle * 3.0f) * 1.0f;

        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(x, y, z));
        transform = glm::rotate(transform, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::scale(transform, glm::vec3(0.015f));

        drawing->add_instance_to_group("cloud", transform);
    }

    drawing->setup_instanced_group("cloud");
}

void MainController::update_clouds() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto drawing = engine::core::Controller::get<engine::drawing::DrawingController>();
    m_cloud_orbit_time += platform->dt();

    float orbit_angle = m_cloud_orbit_time * 10.0f;
    if (m_morbin_time) {
        orbit_angle*=-1;
    }
    for (size_t i = 0; i < m_num_clouds; i++) {
        float angle = (i / (float)m_num_clouds) * 2.0f * M_PI + glm::radians(orbit_angle);

        float x = cos(angle) * m_orbit_radius;
        float z = sin(angle) * m_orbit_radius;
        float y = sin(angle * 3.0f) * 1.0f;

        y += sin(m_cloud_orbit_time * 2.0f + i * 0.5f) * 0.3f;

        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(x, y, z));
        transform = glm::rotate(transform, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::scale(transform, glm::vec3(0.015f));

        drawing->update_instance_in_group("cloud", i, transform);
    }
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::enable_depth_testing();
    engine::graphics::OpenGL::clear_buffers();
}
void MainController::update_camera() {
    auto gui = engine::core::Controller::get<GUIController>();
    if(gui->is_enabled())
        return;

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();


    float dt = platform->dt();
    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_SPACE).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::UP, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt);
    }
}
void MainController::update() {
    update_camera();
    update_lamp_flicker();
    update_clouds();
    m_lamp_is_on = g_lamp_is_on;
    m_lamp_is_flickering = g_lamp_is_flickering;
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("nightsky");
    auto shader = resources->shader("skybox");;
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw() {
    auto vision = engine::core::Controller::get<engine::vision::VisionController>();
    auto drawing = engine::core::Controller::get<engine::drawing::DrawingController>();
    vision->begin_scene();
    draw_skybox();
    //draw_cloud();  // test cloud, non instanced
    drawing->draw(); // instancing
    draw_bed();
    draw_table();
    draw_lamp();

    vision->end_scene();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}



}
