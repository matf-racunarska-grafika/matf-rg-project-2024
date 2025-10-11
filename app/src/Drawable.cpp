//
// Created by nikola on 10/11/25.
//

#include <Drawable.hpp>

#include <engine/resources/ResourcesController.hpp>

namespace app {

Drawable::Drawable() : model(nullptr) {}

Drawable::Drawable(engine::resources::Model* model, engine::resources::Shader* shader)
: model(model) {}

void Drawable::draw(const DirectionalLight& directional_light, const std::vector<PointLight*>& point_lights) {
    model->draw(getShader(directional_light, point_lights));
}
}// app