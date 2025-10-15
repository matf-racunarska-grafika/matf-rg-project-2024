//
// Created by nikola on 10/10/25.
//

#ifndef MATF_RG_PROJECT_MODEL_DRAWABLE_HPP
#define MATF_RG_PROJECT_MODEL_DRAWABLE_HPP

#include <Drawable.hpp>
#include <string>
#include <glm/vec3.hpp>

namespace app {

class PointLight;

class ModelDrawable: public Drawable {
    public:
        engine::resources::Model* model;
        glm::vec3 coordinates;
        glm::vec3 scale;
        float angle;
        glm::vec3 rotation_axis;

        ModelDrawable(const std::string& model_name, const glm::vec3& coordinates, const glm::vec3& scale = glm::vec3(1.0f),
                 float angle = 0, const glm::vec3& rotation_axis = glm::vec3(1,0,0));

        engine::resources::Shader * getShader(const DirectionalLight &directional_light, const std::vector<PointLight*> &point_lights) const;

        void draw(const DirectionalLight &directional_light, const std::vector<PointLight *> &point_lights) override;

        ~ModelDrawable() override;
};
} // app

#endif //MATF_RG_PROJECT_MODEL_DRAWABLE_HPP