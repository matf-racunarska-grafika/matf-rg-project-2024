//
// Created by matija on 10/30/25.
//

#include "../include/engine/resources/LightSwarm.hpp"
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/Shader.hpp>
#include <engine/core/Controller.hpp>
#include <glad/glad.h>

namespace graphics::resources {

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

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());


    CHECKED_GL_CALL(glBindVertexArray, light_vao);

    for (auto &l : lights) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, l.get_position());
        model = glm::scale(model, glm::vec3(m_size));
        shader->set_mat4("model", model);
        CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 36);
    }

    CHECKED_GL_CALL(glBindVertexArray, 0);
}

void LightSwarm::move_lights(float dt) {

    auto rnd = []() -> float {
        // Map rand() in [0, RAND_MAX] to [-1.0, 1.0]
        return 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
    };

    const float scale = m_speed * dt;
    for (auto &l : lights) {
        glm::vec3 delta(2*(std::rand()*1.0f/RAND_MAX)-1, 2*(std::rand()*1.0f/RAND_MAX)-1, 0.3*(2*(std::rand()*1.0f/RAND_MAX)-1));
        l.move_position(delta * scale);
    }
}

} // namespace graphics::resources

