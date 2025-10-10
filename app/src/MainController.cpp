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
void app::MainController::initialize() {
    spdlog::info("KOntroler inicijalizovan");
};
bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}

void MainController::draw_bed() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model* bed = resources->model("bed");
    engine::resources::Shader* shader = resources->shader("basic");
    bed->draw(shader);

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    model = glm::rotate(model, 45.0f, glm::vec3(0.0f,3.0f,0.0f));
    shader->set_mat4("model", model);
        bed->draw(shader);
}

void MainController::begin_draw() {
engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    draw_bed();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
    }


    }// namespace app;
