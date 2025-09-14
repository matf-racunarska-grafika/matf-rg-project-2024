
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::resources {

void Model::set_instanced_draw(glm::mat4 *model_matrix, int amount) { for (auto &mesh: m_meshes) { mesh.set_instanced_draw(model_matrix, amount); } }

void Model::instanced_draw(const Shader *shader, int amount) {
    shader->use();
    for (auto &mesh: m_meshes) { mesh.instanced_draw(shader, amount); }
}

void Model::draw(const Shader *shader) {
    shader->use();
    for (auto &mesh: m_meshes) { mesh.draw(shader); }
}

void Model::destroy() { for (auto &mesh: m_meshes) { mesh.destroy(); } }
}
