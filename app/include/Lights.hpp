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

    void apply(engine::resources::Shader *shader) {}
};

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    void apply(engine::resources::Shader *shader) {}
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

    bool lamp_on;

    void apply(engine::resources::Shader *shader) {}
};

#endif //LIGHTS_HPP
