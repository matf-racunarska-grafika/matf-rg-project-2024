//
// Created by matija on 10/30/25.
//

#include "../include/LightSwarm.hpp"
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/Shader.hpp>
#include <engine/core/Controller.hpp>
#include <glad/glad.h>

namespace app {

LightSwarm::LightSwarm() : light_vao(0), m_speed(1.0f) {
    light_vao = engine::graphics::OpenGL::init_skybox_cube();
}

LightSwarm::~LightSwarm() {
}

void LightSwarm::add_light(const engine::graphics::Light &light) {
    lights.push_back(light);
}

const std::vector<engine::graphics::Light> &LightSwarm::get_lights() const {
    return lights;
}

void LightSwarm::set_lights(const std::vector<engine::graphics::Light> &ls) {
    lights = ls;
}

int LightSwarm::get_light_count() const {
    return lights.size();
}

void LightSwarm::draw(engine::resources::Shader* shader) {

    engine::graphics::GraphicsController* graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    engine::graphics::OpenGL::bindVao(light_vao);

    for (auto &l : lights) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, l.get_position());
        model = glm::scale(model, glm::vec3(m_size));
        shader->set_mat4("model", model);

        shader->set_vec3("lightColor", l.light_data().diffuse);

        engine::graphics::OpenGL::drawArrays(GL_TRIANGLES, 36);
    }

    engine::graphics::OpenGL::bindVao(0);
}

void LightSwarm::move_lights(float dt) {
    const float scale = m_speed * dt;
    for (auto &l : lights) {
        const float rx = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
        const float ry = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
        const float rz = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
        glm::vec3 delta(rx, ry, 0.3f * rz);
        l.move_position(delta * scale);
    }
}
void LightSwarm::set_light_dim(float dim) {
    m_light_dim = dim;
    for (auto light: lights) {
        light.set_brightness(dim);
    }
}

} // namespace app
