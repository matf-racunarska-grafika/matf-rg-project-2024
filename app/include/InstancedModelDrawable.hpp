//
// Created by nikola on 10/16/25.
//

#ifndef MATF_RG_PROJECT_INSTANCEDMODELDRAWABLE_HPP
#define MATF_RG_PROJECT_INSTANCEDMODELDRAWABLE_HPP
#include <Drawable.hpp>

namespace app {

class PointLight;

class InstancedModelDrawable: public Drawable {
    engine::resources::Model* m_model;
    std::vector<glm::mat4> m_model_matrices;
public:
    InstancedModelDrawable(const std::string &m_model, const std::vector<glm::mat4> &m_model_matrices);

    engine::resources::Shader* getShader(const DirectionalLight &directional_light, const std::vector<PointLight*> &point_lights) const;

    void draw(const DirectionalLight &directional_light, const std::vector<PointLight *> &point_lights) override;
};
} // app

#endif //MATF_RG_PROJECT_INSTANCEDMODELDRAWABLE_HPP