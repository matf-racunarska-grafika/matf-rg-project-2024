
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::resources {

void Model::draw(const Shader *shader) {
    shader->use();
    for (auto &mesh: m_meshes) {
        mesh.draw(shader);
    }
}

void Model::destroy() {
    for (auto &mesh: m_meshes) {
        mesh.destroy();
    }
}

void Model::draw_instanced(const Shader *shader, GLsizei amount) {
    shader->use();
    for (auto &mesh: m_meshes) {
        mesh.draw_instanced(shader, amount);
    }
}

void Model::add_instance_vbo(uint32_t buffer_id) {
    for (auto &mesh: m_meshes) {
        mesh.add_instance_vbo(buffer_id);
    }
}
}
