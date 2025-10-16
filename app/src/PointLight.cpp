//
// Created by nikola on 10/11/25.
//

#include <PointLight.hpp>

#include <engine/resources/ResourcesController.hpp>

namespace app {

glm::vec3 PointLight::get_ambient() const {
    if (enabled)
        return ambient;
    return glm::vec3(0.0f);
}

glm::vec3 PointLight::get_diffuse() const {
    if (enabled)
        return diffuse;
    return glm::vec3(0.0f);
}

glm::vec3 PointLight::get_specular() const {
    if (enabled)
        return specular;
    return glm::vec3(0.0f);
}

PointLight::PointLight(const glm::vec3 &ambient, const glm::vec3 &diffuse,
                       const glm::vec3 &specular, const glm::vec3 &light_position, float linear,
                       float quadratic, float shininess, bool enabled)
    : ambient(ambient)
    , diffuse(diffuse)
    , specular(specular)
    , light_position(light_position)
    , linear(linear)
    , quadratic(quadratic)
    , shininess(shininess)
    , enabled(enabled) {}
}// app