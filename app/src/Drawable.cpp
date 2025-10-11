//
// Created by nikola on 10/10/25.
//

#include <Drawable.hpp>

#include <MyController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace app {
    Drawable::Drawable(const std::string& model_name, const std::string& shader_name,
                       const glm::vec3& coordinates, const glm::vec3& scale,
                       float angle, const glm::vec3& rotation_axis)
        : model_name(model_name), shader_name(shader_name), coordinates(coordinates), scale(scale), angle(angle), rotation_axis(rotation_axis){}

    void Drawable::draw(DirectionalLight directional_light, PointLight point_light) {
        //Model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* model = resources->model(model_name);
        //Shader
        engine::resources::Shader* shader = resources->shader(shader_name);
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), coordinates);
        model_matrix = glm::rotate(model_matrix, glm::radians(angle), rotation_axis);
        model_matrix = glm::scale(model_matrix, scale);
        shader->set_mat4("model", model_matrix);

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
} // app