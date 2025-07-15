
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>
#include <glad/glad.h>

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

void Model::set_instance_data(const std::vector<glm::mat4>& instanceMatrices) {
    for (auto& mesh : m_meshes) {
        mesh.setup_instance_buffer(instanceMatrices);
    }
}

void Model::draw_instanced(const Shader* shader, size_t count) {
    shader->use();
    for (auto& mesh : m_meshes) {
        mesh.set_instance_count(static_cast<unsigned int>(count));
        mesh.draw_instanced(shader);
    }
}


}
