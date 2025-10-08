//
// Created by nikola on 10/8/25.
//

#include "../include/MyController.h"

#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"

namespace app {
void MyController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
}
bool MyController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}
void MyController::draw_backpack() {
    //Model
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();
    engine::resources::Model* model = resources->model("backpack");
    //Shader
    engine::resources::Shader* shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("model", glm::mat4(1.0f));
    model->draw(shader);
}
void MyController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MyController::draw() {
    draw_backpack();
}
void MyController::end_draw() {
    get<engine::platform::PlatformController>()->swap_buffers();
}


}// namespace app