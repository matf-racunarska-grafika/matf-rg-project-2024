
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::resources {

void Model::draw(const Shader *shader) {
    shader->use();
    for (auto &mesh: m_meshes) { mesh.draw(shader); }
}

void Model::destroy() { for (auto &mesh: m_meshes) { mesh.destroy(); } }

void Model::setup_instance_matrices(const std::vector<glm::mat4> &instanceMatrices) { for (auto &mesh: m_meshes) mesh.setup_instance_matrices(instanceMatrices); }

void Model::draw_instanced(const Shader *shader, int instanceCount) {
    shader->use();
    for (auto &mesh: m_meshes) mesh.draw_instanced(shader, instanceCount);
}
}
