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
    spdlog::info("Kontroler inicijalizovan");

    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
};
bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}

void MainController::setup_lighting(engine::resources::Shader* shader) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_vec3("viewPos", graphics->camera()->Position);

    shader->set_float("material_shininess", 32.0f);

    shader->set_vec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->set_vec3("dirLight.ambient", glm::vec3(0.4f, 0.4f, 0.5f));   // Higher ambient for visibility
    shader->set_vec3("dirLight.diffuse", glm::vec3(0.6f, 0.6f, 0.7f));   // Brighter diffuse
    shader->set_vec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    shader->set_vec3("pointLights[0].position", glm::vec3(2.0f, 1.5f, 0.0f));
    shader->set_vec3("pointLights[0].ambient", glm::vec3(0.3f, 0.2f, 0.15f));
    shader->set_vec3("pointLights[0].diffuse", glm::vec3(1.0f, 0.8f, 0.6f));
    shader->set_vec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader->set_float("pointLights[0].constant", 1.0f);
    shader->set_float("pointLights[0].linear", 0.09f);
    shader->set_float("pointLights[0].quadratic", 0.032f);

    shader->set_vec3("pointLights[1].position", glm::vec3(-2.0f, 1.5f, 0.0f));
    shader->set_vec3("pointLights[1].ambient", glm::vec3(0.3f, 0.2f, 0.15f));
    shader->set_vec3("pointLights[1].diffuse", glm::vec3(1.0f, 0.8f, 0.6f));
    shader->set_vec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader->set_float("pointLights[1].constant", 1.0f);
    shader->set_float("pointLights[1].linear", 0.09f);
    shader->set_float("pointLights[1].quadratic", 0.032f);

    shader->set_int("numPointLights", 2);
}

void MainController::draw_bed() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model* bed = resources->model("bed");
    engine::resources::Shader* shader = resources->shader("bed");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0);
    shader->set_mat4("model", model);

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
    engine::resources::Shader* shader = resources->shader("bed.glsl");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, m_cloud_transl_factor);
    model = glm::scale(model, m_cloud_scale_factor);
    shader->set_mat4("model", model);

    shader->set_vec3("viewPos", graphics->camera()->Position);
    shader->set_vec3("lightPos", glm::vec3(0.0f, 5.0f, 0.0f));
    shader->set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    cloud->draw(shader);
}

void MainController::begin_draw() {
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
}
void MainController::update() {
    update_camera();
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
    //draw_cloud();


}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
    }




}// namespace app;
