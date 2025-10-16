//
// Created by nikola on 10/16/25.
//

#include <InstancedModelDrawable.hpp>
#include <PointLight.hpp>

#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {

InstancedModelDrawable::InstancedModelDrawable(
    const std::string &model_name,
    const std::vector<glm::mat4> &model_matrices
    ):
m_model_matrices(model_matrices),
m_model(nullptr){
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    m_model = resources->model(model_name);
    m_model->set_instance_matrices(m_model_matrices);
}

engine::resources::Shader *InstancedModelDrawable::getShader(
        const DirectionalLight &directional_light, const std::vector<PointLight *> &point_lights
        ) const {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Shader *shader = resources->shader("instanced");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

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

void InstancedModelDrawable::draw(const DirectionalLight &directional_light, const std::vector<PointLight *> &point_lights) {
    m_model->draw_instanced(getShader(directional_light, point_lights));
}
} // app