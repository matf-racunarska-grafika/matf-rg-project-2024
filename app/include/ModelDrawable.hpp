//
// Created by nikola on 10/10/25.
//

#ifndef MATF_RG_PROJECT_MODEL_DRAWABLE_HPP
#define MATF_RG_PROJECT_MODEL_DRAWABLE_HPP

#include <Drawable.hpp>
#include <string>
#include <glm/mat4x4.hpp>

namespace app {

class PointLight;

class ModelDrawable: public Drawable {
    public:
        engine::resources::Model* model;
        glm::mat4 model_matrix;

        ModelDrawable(const std::string& model_name, const glm::mat4& model_matrix);

        engine::resources::Shader * getShader(const DirectionalLight &directional_light, const std::vector<PointLight*> &point_lights) const;

        void draw(const DirectionalLight &directional_light, const std::vector<PointLight *> &point_lights) override;

        ~ModelDrawable() override;
};
} // app

#endif //MATF_RG_PROJECT_MODEL_DRAWABLE_HPP