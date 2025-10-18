
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::resources {

void Model::draw(const Shader *shader) {
    shader->use();
    for (auto &mesh: m_meshes) {
        mesh.draw(shader);
    }
}

void Model::set_instance_matrices(const std::vector<glm::mat4> &matrices) {
    for(auto& mesh:m_meshes) {
        mesh.setup_instancing(matrices);
    }
}

void Model::draw_instanced(const Shader *shader) {
    shader->use();
    for(auto& mesh:m_meshes) {
        mesh.draw_instanced(shader);
    }
}


void Model::destroy() {
    for (auto &mesh: m_meshes) {
        mesh.destroy();
    }
}
}
