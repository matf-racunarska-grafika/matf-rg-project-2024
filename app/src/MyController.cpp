//
// Created by bojana on 10/8/25.
//

#include "../include/MyController.hpp"

#include "../../engine/libs/glad/include/glad/glad.h"


#include <GL/gl.h>
#include <GUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>


namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    if (!gui_controller->is_enabled()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx,position.dy);
    }
}

void MyController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    auto platform = get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
}
bool MyController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}
void MyController::draw_cottage() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();
    engine::resources::Model* model = resources->model("cottage");

    engine::resources::Shader* shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection",graphics->projection_matrix());
    shader->set_mat4("view",graphics->camera()->view_matrix());
    shader->set_mat4("model",glm::mat4(1.0f));

    // Camera position
    shader->set_vec3("viewPos", graphics->camera()->Position);

    // Directional light
    shader->set_vec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->set_vec3("dirLight.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
    shader->set_vec3("dirLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
    shader->set_vec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    // Point light
    shader->set_vec3("pointLight.position", point_light_position());
    shader->set_vec3("pointLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->set_vec3("pointLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader->set_vec3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader->set_float("pointLight.constant", 1.0f);
    shader->set_float("pointLight.linear", 0.5);
    shader->set_float("pointLight.quadratic", 0.32);

    shader->set_bool("isLightSource", false);

    model->draw(shader);
}


void MyController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    if (!gui_controller->is_enabled()) {
        auto platform = get<engine::platform::PlatformController>();
        auto graphics = get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();

        float dt = platform->dt();
        if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            camera->move_camera(engine::graphics::Camera::FORWARD,dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
            camera->move_camera(engine::graphics::Camera::LEFT,dt);

        }  if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
            camera->move_camera(engine::graphics::Camera::BACKWARD,dt);


        }  if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
            camera->move_camera(engine::graphics::Camera::RIGHT,dt);

        }
    }

}
void MyController::update() {
    update_camera();
}
void MyController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();

}

void MyController::draw_skybox() {
    auto resources = get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("mountain_skybox");
    auto shader = resources->shader("skybox");
    auto graphics = get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader,skybox);

}

void MyController::draw_light_cube() {
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();

    auto shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, point_light_position());
    model = glm::scale(model, glm::vec3(0.4f));
    shader->set_mat4("model", model);

    shader->set_bool("isLightSource", true);
    shader->set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    uint32_t cubeVAO = engine::graphics::OpenGL::init_simple_cube();
    CHECKED_GL_CALL(glBindVertexArray, cubeVAO);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 36);
}

void MyController::draw() {
    draw_cottage();
    draw_light_cube();
    draw_skybox();
}

void MyController::end_draw() {
    auto platform = get<engine::platform::PlatformController>();
    platform->swap_buffers();
}
glm::vec3 MyController::point_light_position() {
    return glm::vec3(0,5,0);
}


}// namespace app