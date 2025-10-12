//
// Created by matfrg on 10/11/25.
//

#include "MainController.hpp"

#include "spdlog/spdlog.h"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {

void MainController::initialize() {
    spdlog::info("MainController initialized");
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
    engine::resources::Model* island=resources->model("ostrvo");
    //Shader
    engine::resources::Shader* shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection",graphics->projection_matrix());
    shader->set_mat4("view",graphics->camera()->view_matrix());
    glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,-5));
    model = glm::scale(model,glm::vec3(0.3f));
    shader->set_mat4("model",model);

    island->draw(shader);

}
void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}


void MainController::draw() {
    //clear buffers (color buffer, depth buffer)
    draw_island();
    //swap buffers
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}


}// namespace app
