//
// Created by spaske on 12/2/24.
//
#include<glad/glad.h>
#include <engine/util/Utils.hpp>
#include <engine/render/Mesh.hpp>
#include <engine/render/Shader.hpp>

namespace rg {

    Mesh Mesh::create(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
                      const std::unordered_set<Texture *> &textures) {
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex_cords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));

        glBindVertexArray(0);

        return {VAO, indices.size(), std::vector(range(textures))};
    }

    void Mesh::initialize() {
    }

    void Mesh::draw(ShaderProgram *shader) {
        std::unordered_map<std::string_view, uint32_t> counts;
        std::string uniform_name;
        uniform_name.reserve(32);
        for (int i = 0; i < m_textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            const auto &texture_type = Texture::uniform_name_convention(m_textures[i]->type());
            uniform_name.append(texture_type);
            const auto count = (counts[texture_type] += 1);
            uniform_name.append(std::to_string(count));
            shader->set_int(uniform_name, i);
            glBindTexture(GL_TEXTURE_2D, m_textures[i]->id());
            uniform_name.clear();
        }
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::destroy() {
        glDeleteVertexArrays(1, &m_vao);
    }

}