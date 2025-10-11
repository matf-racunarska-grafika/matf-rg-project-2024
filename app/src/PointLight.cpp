//
// Created by nikola on 10/11/25.
//

#include <PointLight.hpp>

#include <engine/resources/ResourcesController.hpp>

namespace app {

PointLight::~PointLight() {}

glm::vec3 PointLight::light_position() const { return coordinates + relative_light_position; }

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

PointLight::PointLight(const std::string &model_name, const glm::vec3 &coordinates, const glm::vec3 &scale, const float angle,
                       const glm::vec3 &rotation_axis, const glm::vec3 &ambient, const glm::vec3 &diffuse,
                       const glm::vec3 &specular, const glm::vec3 &relative_position, float linear, float quadratic, float shininess, bool enabled
        ) : ModelDrawable(model_name, coordinates, scale, angle, rotation_axis)
        , ambient(ambient)
        , diffuse(diffuse)
        , specular(specular)
        , relative_light_position(relative_position)
        , linear(linear)
        , quadratic(quadratic)
        , shininess(shininess)
        , enabled(enabled) {}
}// app