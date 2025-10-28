#include <glad/glad.h>
#include <engine/util/Utils.hpp>
#include <engine/resources/Mesh.hpp>
#include <engine/resources/Shader.hpp>
#include <unordered_map>

namespace engine::resources {

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
           std::vector<Texture *> textures) {
    static_assert(std::is_trivial_v<Vertex>);
    uint32_t VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position));


    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));


    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));


    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Tangent));


    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Bitangent));

    glBindVertexArray(0);

    m_vao = VAO;
    m_num_indices = static_cast<uint32_t>(indices.size());
    m_textures = std::move(textures);
}

void Mesh::draw(const Shader *shader) {
    uint32_t textureUnit = 0;
    std::unordered_map<TextureType, uint32_t> typeCounts;

    for (auto *texture: m_textures) {
        if (!texture) { continue; }

        glActiveTexture(GL_TEXTURE0 + textureUnit);

        std::string_view baseName = Texture::uniform_name_convention(texture->type());

        uint32_t typeIndex = typeCounts[texture->type()] + 1;
        typeCounts[texture->type()] = typeIndex;

        std::string uniformName = std::string(baseName) + std::to_string(typeIndex);

        shader->set_int(uniformName, textureUnit);
        glBindTexture(GL_TEXTURE_2D, texture->id());

        textureUnit++;
    }

    for (uint32_t i = textureUnit; i < 8; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, (GLsizei) m_num_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

uint32_t Mesh::attach_instance_matrices(const std::vector<glm::mat4> &mats,
                                        unsigned baseAttribLocation) const {
    uint32_t vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mats.size() * sizeof(glm::mat4), mats.data(), GL_STATIC_DRAW);

    glBindVertexArray(m_vao);

    const std::size_t vec4Size = sizeof(glm::vec4);
    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(baseAttribLocation + i);
        glVertexAttribPointer(baseAttribLocation + i, 4, GL_FLOAT, GL_FALSE,
                              sizeof(glm::mat4), (void *) (i * vec4Size));
        glVertexAttribDivisor(baseAttribLocation + i, 1);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vbo;
}

void Mesh::draw_instanced(const Shader *shader, uint32_t instanceCount) const {
    uint32_t textureUnit = 0;
    std::unordered_map<TextureType, uint32_t> typeCounts;

    for (auto *texture: m_textures) {
        if (!texture) { continue; }

        glActiveTexture(GL_TEXTURE0 + textureUnit);

        std::string_view baseName = Texture::uniform_name_convention(texture->type());
        uint32_t typeIndex = typeCounts[texture->type()] + 1;
        typeCounts[texture->type()] = typeIndex;
        std::string uniformName = std::string(baseName) + std::to_string(typeIndex);

        shader->set_int(uniformName, textureUnit);
        glBindTexture(GL_TEXTURE_2D, texture->id());

        textureUnit++;
    }

    for (uint32_t i = textureUnit; i < 8; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(m_vao);
    glDrawElementsInstanced(GL_TRIANGLES,
                            (GLsizei) m_num_indices,
                            GL_UNSIGNED_INT, 0,
                            (GLsizei) instanceCount);
    glBindVertexArray(0);
}

void Mesh::destroy() {
    glDeleteVertexArrays(1, &m_vao);
}

}// namespace engine::resources