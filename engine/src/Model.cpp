
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>
#include "glad/glad.h"

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

void Model::draw_instanced(const Shader *shader, const std::vector<glm::mat4> &model_matrices) {
    const int instances_amount = model_matrices.size();
    unsigned int buffer;

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, instances_amount * sizeof(glm::mat4), model_matrices.data(), GL_STATIC_DRAW);


    for (auto &m: this->m_meshes) {
        const unsigned int VAO = m.get_vao();
        glBindVertexArray(VAO);
        const std::size_t size = sizeof(glm::vec4);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * size, (void *) 0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * size, (void *) (size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * size, (void *) (2 * size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * size, (void *) (3 * size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    shader->use();
    for (auto &mesh: m_meshes) { mesh.draw_instanced(shader, instances_amount); }
}
}
