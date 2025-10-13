
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::resources {

void Model::draw(const Shader *shader) {
    shader->use();
    for (auto &mesh: m_meshes) {
        if (mesh.is_instanced()) {
            mesh.draw_instanced(shader);
        }
        else {
            mesh.draw(shader);
        }
    }
}

void Model::destroy() {
    for (auto &mesh: m_meshes) {
        mesh.destroy();
    }
}

void Model::instancite(glm::mat4 *transform, uint32_t count) {
    for (auto &mesh: m_meshes) {
        if (!mesh.is_instanced())
            mesh.instanciate(transform, count);
    }
}
}
