//
// Created by nikola on 10/11/25.
//

#ifndef MATF_RG_PROJECT_DIRECTIONALLIGHT_HPP
#define MATF_RG_PROJECT_DIRECTIONALLIGHT_HPP

#include <glm/vec3.hpp>

namespace app {

struct DirectionalLight {
    glm::vec3 direction = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec3 ambient = glm::vec3(0.3f);
    glm::vec3 diffuse = glm::vec3(0.7f);
    glm::vec3 specular = glm::vec3(0.5f);
    float shininess = 8.0f;
};

} // app

#endif //MATF_RG_PROJECT_DIRECTIONALLIGHT_HPP