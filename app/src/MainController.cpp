//
// Created by aleksa on 1.6.25..
//

#include "MainController.h"
#include "../../engine/libs/glad/include/glad/glad.h"

#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/resources/ResourcesController.hpp>

#include "spdlog/spdlog.h"
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>

namespace app {
void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    spdlog::info("MainController initialized");
};

bool MainController::loop() {
    auto platform = get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).is_down()) { return false; }
    return true;
}

void MainController::draw_cat() {
    //  Model
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();

    engine::resources::Model *cat = resources->model("football");
    //  Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));

    model = glm::scale(model, glm::vec3(0.02f));
    shader->set_mat4("model", model);
    cat->draw(shader);
}

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::end_draw() {
    auto platform = get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MainController::draw() { draw_cat(); }

}// app