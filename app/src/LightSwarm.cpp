//
// Created by matija on 10/30/25.
//

#include "../include/LightSwarm.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
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
    set_light_dim(0.4f);
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
    if (m_light_dim<0.03f) return;
    engine::graphics::GraphicsController* graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->use();
    graphics->prepare_for_draw(shader);
    engine::graphics::OpenGL::bindVao(light_vao);

    for (auto &l : lights) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, l.get_position());
        model = glm::scale(model, glm::vec3(m_size));
        shader->set_mat4("model", model);

        shader->set_vec3("lightColor", l.light_data().specular);

        engine::graphics::OpenGL::drawArrays(GL_TRIANGLES, 36);
    }

    engine::graphics::OpenGL::bindVao(0);
}

void LightSwarm::move_lights(float dt) {
    float z_squish=0.3;
    float detour_scale=0.1f;
    const float scale = m_speed * dt;
    for (auto &l : lights) {

        l.move_position( l.get_direction() * scale );

        const float rx = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
        const float ry = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
        const float rz = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;

        glm::vec3 b(rx,ry *z_squish ,rz);
        b=normalize(b);
        if (glm::dot(l.get_direction(), b) < 0.0f)
            b=-b;

        glm::vec3 apos= glm::normalize(l.get_position());
        glm::vec3 bdir=l.get_direction() +  b * detour_scale ;
        bdir=normalize(bdir);

        glm::vec3 bdirproj = (glm::dot(bdir, apos) / glm::dot(apos, apos)) * apos;
        glm::vec3 bdir_ortho = bdir - bdirproj;

        l.set_direction( normalize( bdir_ortho ) );
    }
}
void LightSwarm::set_light_dim(float dim) {
    m_light_dim = dim;
    for (auto& light: lights) {
        light.set_brightness(dim);
    }
}

} // namespace app
