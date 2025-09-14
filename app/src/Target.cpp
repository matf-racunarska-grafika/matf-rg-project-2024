//
// Created by boce on 9/14/25.
//

#include "../include/Target.hpp"

#include "engine/core/App.hpp"
#include "engine/core/Controller.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/resources/ResourcesController.hpp"

namespace app {

Target::Target(engine::resources::Model *model, const glm::vec3 &position) {
    m_model = model;
    m_position = position;
}


void Target::draw(const engine::resources::Shader *shader, const DirectionalLight &dirlight, const SpotLight &spotlight) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_position);
    model = glm::rotate(model, glm::radians(m_angle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(m_scale));
    shader->set_mat4("model", model);
    glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
    shader->set_mat3("invNormal", normal_matrix);

    shader->set_float("shininess", 32.0f);
    shader->set_vec3("viewPos", graphics->camera()->Position);

    dirlight.apply(shader, "dirlight");
    spotlight.apply(shader, "spotlight");

    m_model->draw(shader);
}


}// app