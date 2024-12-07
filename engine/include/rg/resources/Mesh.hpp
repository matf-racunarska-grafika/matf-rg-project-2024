
//

#ifndef MATF_RG_PROJECT_MESH_HPP
#define MATF_RG_PROJECT_MESH_HPP

#include <glm/glm.hpp>
#include <vector>
#include <rg/resources/Texture.hpp>

namespace rg {
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;

        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    class Mesh {
    public:
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
             std::vector<Texture *> textures);

        void initialize();

        void draw(const Shader *shader);

        void destroy();

        std::string_view name() const {
            return "Mesh";
        }

    private:
        Mesh(uint32_t id, uint64_t num_indices, std::vector<Texture *> textures) : m_vao(id)
          , m_num_indices(num_indices)
          , m_textures(std::move(textures)) {
        }

        uint32_t m_vao         = 0;
        uint64_t m_num_indices = 0;
        std::vector<Texture *> m_textures;

        static std::string_view texture_type_to_uniform_name_convention(TextureType type);
    };
} // namespace rg

#endif//MATF_RG_PROJECT_MESH_HPP
