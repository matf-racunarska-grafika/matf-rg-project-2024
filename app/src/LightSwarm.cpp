//
// Created by matija on 10/30/25.
//

#include <LightSwarm.hpp>
#include <cstdlib>
#include <ctime>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/Shader.hpp>
#include <engine/core/Controller.hpp>

namespace app {

LightSwarm::LightSwarm() : m_light_vao(0), m_speed(1.0f) {
    m_light_vao = engine::graphics::OpenGL::init_skybox_cube();
}

LightSwarm::~LightSwarm() {
}

void LightSwarm::add_light(const engine::graphics::Light &light) {
    m_lights.push_back(light);
}

const std::vector<engine::graphics::Light> &LightSwarm::lights() const {
    return m_lights;
}

void LightSwarm::set_lights(const std::vector<engine::graphics::Light> &ls) {
    m_lights = ls;
}

int LightSwarm::light_count() const { return static_cast<int>(m_lights.size()); }

void LightSwarm::draw(engine::resources::Shader* shader) {
    if (m_light_dim<0.03f) return;
    auto* graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->use();
    graphics->prepare_for_draw(shader);
    engine::graphics::OpenGL::bind_vao(m_light_vao);

    for (auto &l : m_lights) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, l.get_position());
        model = glm::scale(model, glm::vec3(m_size));
        shader->set_mat4("model", model);
        shader->set_vec3("lightColor", l.light_data().specular);
        engine::graphics::OpenGL::draw_arrays(GL_TRIANGLES, 36);
    }

    engine::graphics::OpenGL::bind_vao(0);
}

void LightSwarm::move_lights(float dt) {
    float z_squish=0.5f;
    float detour_scale=0.05f;
    const float scale = m_speed * dt;
    for (auto &l : m_lights) {
        l.move_position( l.get_direction() * scale );
        const float rx = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
        const float ry = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
        const float rz = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
        glm::vec3 b(rx,ry *z_squish ,rz);
        b=normalize(b);
        if (glm::dot(l.get_direction(), b) < 0.0f) b=-b;
        glm::vec3 apos= glm::normalize(l.get_position()-glm::vec3(0,0.3,0));
        glm::vec3 bdir=l.get_direction() +  b * detour_scale ;
        bdir=normalize(bdir);
        glm::vec3 bdirproj = (glm::dot(bdir, apos) / glm::dot(apos, apos)) * apos;
        glm::vec3 bdir_ortho = bdir - bdirproj;
        l.set_direction( normalize( bdir_ortho ) );
    }
}
void LightSwarm::set_light_dim(float dim) {
    m_light_dim = dim;
    for (auto& light: m_lights) {
        light.set_brightness(dim);
    }
}

} // namespace app
