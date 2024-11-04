
//

#ifndef MATF_RG_PROJECT_MESH_HPP
#define MATF_RG_PROJECT_MESH_HPP

#include <glm/glm.hpp>
#include <vector>
#include <engine/render/Texture.hpp>

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

#ifdef PLATFORM_OPENGL
    struct Mesh {
        std::vector<Texture *> textures;
        uint32_t m_id;
    };
#elif PLATFORM_VULKAN

#endif
}// namespace rg

#endif//MATF_RG_PROJECT_MESH_HPP
