//
// Created by nikola on 10/10/25.
//

#ifndef MATF_RG_PROJECT_DRAWABLE_HPP
#define MATF_RG_PROJECT_DRAWABLE_HPP

#include <MyController.hpp>
#include <string>
#include <glm/vec3.hpp>

namespace app {
struct DirectionalLight;
struct PointLight;

class Drawable {
    public:
        std::string model_name;
        std::string shader_name;
        glm::vec3 coordinates;
        glm::vec3 scale;

        Drawable(const std::string& model_name, const std::string& shader_name,
                 const glm::vec3& coordinates, const glm::vec3& scale = glm::vec3(1.0f));

        void draw(DirectionalLight directional_light, PointLight point_light);
    };
} // app

#endif //MATF_RG_PROJECT_DRAWABLE_HPP