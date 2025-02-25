
#include <glad/glad.h>
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::resources {

    void Model::draw(const Shader *shader) {
        shader->use();
        for (auto &mesh: m_meshes) {
            mesh.draw(shader);
        }
    }

    void Model::drawBlended(const Shader *shader) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        draw(shader);
        glDisable(GL_BLEND);
    }

    void Model::drawInstanced(const Shader *shader, unsigned int number_of_instances) {
        shader->use();
        for (auto &mesh: m_meshes) {
            mesh.drawInstanced(shader, number_of_instances);
        }
    }

    void Model::destroy() {
        for (auto &mesh: m_meshes) {
            mesh.destroy();
        }
    }
}
