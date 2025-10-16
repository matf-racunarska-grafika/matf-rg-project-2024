//
// Created by nikola on 10/10/25.
//

#include <ModelDrawable.hpp>

#include <MyController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace app {

ModelDrawable::ModelDrawable(const std::string &model_name, const glm::mat4 &model_matrix)
    : model(nullptr)
    , model_matrix(model_matrix) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    model = resources->model(model_name);
}

engine::resources::Shader *ModelDrawable::getShader(
        const DirectionalLight &directional_light, const std::vector<PointLight *> &point_lights
        ) const {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("model", model_matrix);

    shader->set_vec3("directional_light.direction", directional_light.direction);
    shader->set_vec3("directional_light.ambient", directional_light.ambient);
    shader->set_vec3("directional_light.diffuse", directional_light.diffuse);
    shader->set_vec3("directional_light.specular", directional_light.specular);
    shader->set_float("directional_light.shininess", directional_light.shininess);

    shader->set_int("point_light_count", point_lights.size());
    for (int i = 0; i < point_lights.size(); i++) {
        shader->set_vec3("point_lights[" + std::to_string(i) + "].position", point_lights[i]->light_position);
        shader->set_vec3("point_lights[" + std::to_string(i) + "].ambient", point_lights[i]->get_ambient());
        shader->set_vec3("point_lights[" + std::to_string(i) + "].diffuse", point_lights[i]->get_diffuse());
        shader->set_vec3("point_lights[" + std::to_string(i) + "].specular", point_lights[i]->get_specular());
        shader->set_float("point_lights[" + std::to_string(i) + "].linear", point_lights[i]->linear);
        shader->set_float("point_lights[" + std::to_string(i) + "].quadratic", point_lights[i]->quadratic);
        shader->set_float("point_lights[" + std::to_string(i) + "].shininess", point_lights[i]->shininess);
    }
    return shader;
}

void ModelDrawable::draw(const DirectionalLight &directional_light, const std::vector<PointLight *> &point_lights) {
    model->draw(getShader(directional_light, point_lights));
}

ModelDrawable::~ModelDrawable() = default;
}// app