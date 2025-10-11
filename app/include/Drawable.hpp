//
// Created by nikola on 10/11/25.
//

#ifndef MATF_RG_PROJECT_DRAWABLE_HPP
#define MATF_RG_PROJECT_DRAWABLE_HPP
#include <DirectionalLight.hpp>
#include <engine/resources/Model.hpp>
#include <vector>

namespace app {

class PointLight;

class Drawable {
    public:
        virtual ~Drawable() {}

        engine::resources::Model* model;

        Drawable();
        Drawable(engine::resources::Model* model, engine::resources::Shader* shader);

        virtual engine::resources::Shader* getShader(const DirectionalLight& directional_light, const std::vector<PointLight*>& point_lights) = 0;

        void draw(const DirectionalLight& directional_light, const std::vector<PointLight*>& point_lights);
    };
} // app

#endif //MATF_RG_PROJECT_DRAWABLE_HPP