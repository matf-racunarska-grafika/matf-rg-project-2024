
//

#ifndef MATF_RG_PROJECT_MESH_HPP
#define MATF_RG_PROJECT_MESH_HPP

#include <glm/glm.hpp>
#include <vector>
#include <engine/render/Texture.hpp>
#include <unordered_set>

namespace rg {
    struct Vertex;
    struct VertexSOA;

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_cords;

        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    struct VertexSOA {
        std::vector<glm::vec3> position;
        std::vector<glm::vec3> normal;
        std::vector<glm::vec2> tex_cords;

        std::vector<glm::vec3> tangent;
        std::vector<glm::vec3> bitangent;
    };

    VertexSOA AOS_vertex_to_SOA(const std::vector<Vertex> &aos);
    std::vector<Vertex> SOA_vertex_to_AOS(const VertexSOA &soa);

    // TODO(mspasic): add value operations so that meshes cleanup automatically after they are no longer needed
    class Mesh : public DrawableElement {
    public:
        static Mesh create(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
                           const std::unordered_set<Texture *> &textures);

        void initialize() override {
        }

        void draw(ShaderProgram *shader) override;

        void destroy() override {
        }

        std::string_view name() const override {
            return "Mesh";
        }

    private:
        Mesh(uint32_t id, std::vector<Texture *> textures) : m_id(id), m_textures(std::move(textures)) {
        }
        uint32_t m_id = 0;
        std::vector<Texture *> m_textures;
    };

    struct PlatformMeshInitializationData;


}// namespace rg

#endif//MATF_RG_PROJECT_MESH_HPP
