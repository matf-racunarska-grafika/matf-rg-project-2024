//
// Created by nikola on 10/8/25.
//

#include "../include/MyController.hpp"

#include "GUIController.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"

namespace app {
class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui = engine::core::Controller::get<GUIController>();
    if (!gui->is_enabled()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }
}


void MyController::initialize() {
    auto platform = get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
}
bool MyController::loop() {
    auto platform = get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}
void MyController::draw_backpack() {
    //Model
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();
    engine::resources::Model* model = resources->model("building");
    //Shader
    engine::resources::Shader* shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(0.05f)));

    shader->set_vec3("directional_light.direction", directional_light.direction);
    shader->set_vec3("directional_light.ambient", directional_light.ambient);
    shader->set_vec3("directional_light.diffuse", directional_light.diffuse);
    shader->set_vec3("directional_light.specular", directional_light.specular);
    shader->set_float("directional_light.shininess", directional_light.shininess);
    shader->set_vec3("directional_light.intensity", directional_light.intensity);

    shader->set_vec3("point_light.position", point_light.position);
    shader->set_vec3("point_light.ambient", point_light.ambient);
    shader->set_vec3("point_light.diffuse", point_light.diffuse);
    shader->set_vec3("point_light.specular", point_light.specular);
    shader->set_float("point_light.linear", point_light.linear);
    shader->set_float("point_light.quadratic", point_light.quadratic);
    shader->set_vec3("point_light.intensity", point_light.intensity);

    model->draw(shader);
}
void MyController::update_camera() {
    auto gui_controller = get<GUIController>();
    if (!gui_controller->is_enabled()) {
        auto platform = get<engine::platform::PlatformController>();
        auto graphics = get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        auto dt = platform->dt();
        if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
        }
    }
}
void MyController::update() {
    update_camera();
}
void MyController::draw_skybox() {
    auto resources = get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("mountain_skybox");
    auto shader = resources->shader("skybox");
    auto graphics = get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}
void MyController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MyController::draw() {
    draw_backpack();
    draw_skybox();
}
void MyController::end_draw() {
    get<engine::platform::PlatformController>()->swap_buffers();
}
}// namespace app