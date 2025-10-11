//
// Created by nikola on 10/11/25.
//

#ifndef MATF_RG_PROJECT_POINTLIGHT_HPP
#define MATF_RG_PROJECT_POINTLIGHT_HPP
#include <ModelDrawable.hpp>
#include <string>
#include <glm/vec3.hpp>

namespace app {

class PointLight : public ModelDrawable {

public:
    ~PointLight() override;

    glm::vec3 ambient = glm::vec3(0.3, 0, 0);
    glm::vec3 diffuse = glm::vec3(0.6f);
    glm::vec3 specular = glm::vec3(0.4f);
    glm::vec3 relative_light_position = glm::vec3(0.0f);
    float linear = 0.02f;
    float quadratic = 0.01f;
    float shininess = 32.0f;

    glm::vec3 light_position() const;

    PointLight(
        const std::string &model_name, const glm::vec3 &coordinates ,const glm::vec3 &scale, float angle,
        const glm::vec3 &rotation_axis, const glm::vec3 &ambient, const glm::vec3 &diffuse,
        const glm::vec3 &specular, const glm::vec3 &relative_position, float linear, float quadratic, float shininess);
};
}// app

#endif //MATF_RG_PROJECT_POINTLIGHT_HPP