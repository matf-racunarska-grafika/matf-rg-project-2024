//
// Created by zesla on 10/9/25.
//

#include <MainController.hpp>
#include <engine/core/App.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
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
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx,position.dy);
}

void app::MainController::initialize() {
    spdlog::info("Controller initiallized!");

    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));

    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->Position = glm::vec3(-7.0f, 7.0f, 7.0f);
};
bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    if (platform->key(engine::platform::KeyId::KEY_L).state() == engine::platform::Key::State::JustPressed) {
        if (!lamp_is_flickering) {
            lamp_is_on = !lamp_is_on;
            spdlog::info("Lamp has just been (manually) turned {}", lamp_is_on?"on!":"off!");
        }
    }
    return true;
}

void MainController::setup_lighting(engine::resources::Shader* shader) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_vec3("viewPos", graphics->camera()->Position);

    //TODO:shininess

    shader->set_vec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->set_vec3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->set_vec3("dirLight.diffuse", glm::vec3(0.2f, 0.2f, 0.3f));
    shader->set_vec3("dirLight.specular", glm::vec3(0.2f, 0.2f, 0.2f));

    shader->set_vec3("pointLights[0].ambient", glm::vec3(0)); //0.3f, 0.2f, 0.15f
    shader->set_vec3("pointLights[0].diffuse", glm::vec3(0)); //1.0f, 0.8f, 0.6f
    shader->set_vec3("pointLights[0].specular", glm::vec3(0)); //1.0f, 1.0f, 1.0f
    shader->set_float("pointLights[0].constant", 1.0f);
    shader->set_float("pointLights[0].linear", 0.09f);
    shader->set_float("pointLights[0].quadratic", 0.032f);

    if (lamp_is_on) {
        shader->set_vec3("pointLights[1].position", lamp_position + glm::vec3(0.0f, 0.5f, 0.0f));
        shader->set_vec3("pointLights[1].ambient", glm::vec3(0.4f, 0.35f, 0.3f));
        shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.7f, 0.6f));
        shader->set_vec3("pointLights[1].ambient", glm::vec3(0.2f, 0.18f, 0.15f));
        shader->set_vec3("pointLights[1].specular", glm::vec3(0.5f, 0.45f, 0.4f));
        shader->set_float("pointLights[1].constant", 1.0f);
        shader->set_float("pointLights[1].linear", 0.22f);
        shader->set_float("pointLights[1].quadratic", 0.20f);
    } else {
        shader->set_vec3("pointLights[1].position", lamp_position);
        shader->set_vec3("pointLights[1].ambient", glm::vec3(0.0f));
        shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.0f));
        shader->set_vec3("pointLights[1].specular", glm::vec3(0.0f));
        shader->set_float("pointLights[1].constant", 1.0f);
        shader->set_float("pointLights[1].linear", 1.0f);
        shader->set_float("pointLights[1].quadratic", 1.0f);
    }

    shader->set_int("numPointLights", 2);
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

    engine::resources::Model* cloud = resources->model("clouds");
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

    model = glm::translate(model, lamp_position);

    model = glm::scale(model, glm::vec3(0.09f, 0.09f, 0.09f));
    shader->set_mat4("model", model);

    shader->set_int("lampIsOn", lamp_is_on ? 1 : 0);
    shader->set_vec3("viewPos", graphics->camera()->Position);
    shader->set_float("material_shininess", 64.0f);

    setup_lighting(shader);

    lamp->draw(shader);
}

void MainController::update_lamp_flicker() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();

    if (g_start_flicker_sequence) {
        start_flicker_sequence = true;
        g_start_flicker_sequence = false;
    }

    if (start_flicker_sequence && !lamp_is_flickering) {
        start_flicker_sequence = false;
        flicker_delay_timer = 3.0f;
        lamp_is_flickering = true;
        spdlog::info("Flicker sequence started - waiting 3 seconds");
    }

    if (lamp_is_flickering) {
        if (flicker_delay_timer > 0.0f) {
            flicker_delay_timer -= dt;

            if (flicker_delay_timer <= 0.0f) {
                flicker_timer = 3.0f;
                flicker_interval = 0.0f;
                spdlog::info("Starting flicker effect");
            }
        }
        else if (flicker_timer > 0.0f) {
            flicker_timer -= dt;
            g_flicker_timer = flicker_timer;

            flicker_interval -= dt;
            if (flicker_interval <= 0.0f) {
                lamp_is_on = !lamp_is_on;
                g_lamp_is_on = lamp_is_on;

                flicker_interval = 0.05f + (rand() % 150) / 1000.0f;
            }

            if (flicker_timer <= 0.0f) {
                lamp_is_on = (rand() % 2) == 0;  // 50/50 chance
                g_lamp_is_on = lamp_is_on;
                lamp_is_flickering = false;
                g_lamp_is_flickering = false;

                spdlog::info("Flicker effect ended - lamp is now (due to pure luck): {}", lamp_is_on ? "on!" : "off!");
            }
        }
    }

    g_lamp_is_flickering = lamp_is_flickering;
    g_lamp_is_on = lamp_is_on;
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::enable_depth_testing();
engine::graphics::OpenGL::clear_buffers();
}
void MainController::update_camera() {

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
    lamp_is_on = g_lamp_is_on;
    lamp_is_flickering = g_lamp_is_flickering;
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("nightsky");
    auto shader = resources->shader("skybox");;
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw() {
    draw_skybox();
    draw_bed();
    draw_cloud();
    draw_table();
    draw_lamp();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}




}// namespace app;
