//
// Created by matfrg on 10/11/25.
//

#include "MainController.hpp"

#include "AppUtils.hpp"
#include "GuiController.hpp"
#include "spdlog/spdlog.h"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {
class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {

public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    if(is_gui_active()) {
        return;
    }
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx,position.dy);
}


void MainController::initialize() {
    spdlog::info("MainController initialized");
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    platform->set_enable_cursor(false);
    engine::graphics::OpenGL::enable_depth_testing();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}

void MainController::draw_island() {
    //Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *island = resources->model("ostrvo");
    //Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -2, 1));
    //-0.95 1.27 2.02
    model = glm::scale(model, glm::vec3(0.7f));
    shader->set_mat4("model", model);

    island->draw(shader);
}

void MainController::draw_model(const std::string &model_name,
    const std::string &shader_name,
    const glm::vec3 &position,
    const glm::vec3 &scale,
    const glm::vec3 &rotation_axis,
    float rotation_angle) {

    //Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *model = resources->model(model_name);
    //Shader
    engine::resources::Shader *shader = resources->shader(shader_name);

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), position);
    //-0.95 1.27-1.2 2.02

    if(rotation_angle!=0.0f && glm::length(rotation_axis)>0.0f) {
        model_matrix = glm::rotate(model_matrix,glm::radians(rotation_angle),rotation_axis);
    }

    model_matrix = glm::scale(model_matrix, scale);
    shader->set_mat4("model", model_matrix);

    model->draw(shader);




}

void MainController::update_camera() {

    if(is_gui_active()) {
        return;
    }

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();
    if(platform->key(engine::platform::KeyId::KEY_W).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD,dt);
    }
    if(platform->key(engine::platform::KeyId::KEY_A).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT,dt);
    }
    if(platform->key(engine::platform::KeyId::KEY_S).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD,dt);
    }
    if(platform->key(engine::platform::KeyId::KEY_D).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT,dt);
    }
}


void MainController::update() {
    update_camera();
}


void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("space_skybox");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader,skybox);
}


void MainController::draw() {
    draw_island();

    draw_model("zvezda","zvezda",glm::vec3(-0.93,1.44,2.02)
        ,glm::vec3(0.1f));
    draw_model("kuca","basic",glm::vec3(1.1,0,0.37)
        ,glm::vec3(0.1f),glm::vec3(0,1,0),-45.0);
    draw_model("snesko","basic",glm::vec3(-0.27,0.04,2.87)
        ,glm::vec3(0.1f),glm::vec3(0,1,0),165.0);
    draw_model("sanke","basic",glm::vec3(-1.33,-0.01,0.64)
        ,glm::vec3(0.1f));
    draw_model("poklon","basic",glm::vec3(-0.88,-0.01,1.42)
        ,glm::vec3(0.008f),glm::vec3(1,0,0),-90);
    draw_model("poklon","basic",glm::vec3(-1.2,-0.01,1.71)
        ,glm::vec3(0.008f),glm::vec3(1,0,0),-90);
    draw_model("bonfire","basic_rgba",glm::vec3(-0.5,-0.01,0.18)
        ,glm::vec3(0.05f));
    draw_skybox();

}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}


}// namespace app
