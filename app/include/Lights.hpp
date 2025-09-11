//
// Created by boce on 9/10/25.
//

#ifndef LIGHTS_HPP
#define LIGHTS_HPP
#include "engine/resources/ResourcesController.hpp"
#include "glm/vec3.hpp"

struct DirectionalLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    void apply(const engine::resources::Shader *shader, const std::string &name) const {
        shader->use();
        shader->set_vec3(name + ".direction", direction);
        shader->set_vec3(name + ".ambient", ambient);
        shader->set_vec3(name + ".diffuse", diffuse);
        shader->set_vec3(name + ".specular", specular);
    }
};

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    void apply(const engine::resources::Shader *shader, const std::string &name) const {
        shader->use();
        shader->set_vec3(name + ".position", position);
        shader->set_vec3(name + ".ambient", ambient);
        shader->set_vec3(name + ".diffuse", diffuse);
        shader->set_vec3(name + ".specular", specular);
        shader->set_float(name + ".constant", constant);
        shader->set_float(name + ".linear", linear);
        shader->set_float(name + ".quadratic", quadratic);
    }
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;

    float inner_cut_off;
    float outer_cut_off;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    int lamp_on;

    void apply(const engine::resources::Shader *shader, const std::string &name) const {
        shader->use();
        shader->set_vec3(name + ".direction", direction);
        shader->set_vec3(name + ".position", position);
        shader->set_vec3(name + ".ambient", ambient);
        shader->set_vec3(name + ".diffuse", diffuse);
        shader->set_vec3(name + ".specular", specular);
        shader->set_float(name + ".constant", constant);
        shader->set_float(name + ".linear", linear);
        shader->set_float(name + ".quadratic", quadratic);
        shader->set_float(name + ".inner_cut_off", inner_cut_off);
        shader->set_float(name + ".outer_cut_off", outer_cut_off);
        shader->set_int(name + ".lamp_on", lamp_on);
    }
};

#endif //LIGHTS_HPP
