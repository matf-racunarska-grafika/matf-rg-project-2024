
//

#ifndef MATF_RG_PROJECT_MESH_HPP
#define MATF_RG_PROJECT_MESH_HPP

#include <glm/glm.hpp>
#include <vector>
#include <engine/resources/Texture.hpp>

namespace engine::resources {
    /**
    * @struct Vertex
    * @brief Represents a vertex in the mesh.
    */
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;

        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    /**
    * @class Mesh
    * @brief Represents a mesh in the model in the OpenGL context.
    */
    class Mesh {
    public:
        /**
        * @brief Constructs a Mesh object.
        * @param vertices The vertices in the mesh.
        * @param indices The indices in the mesh.
        * @param textures The textures in the mesh.
        */
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
             std::vector<Texture *> textures);

        void initialize();

        /**
        * @brief Draws the mesh using a given shader. Called by the @ref Model::draw function to draw all the meshes in the model.
        * @param shader The shader to use for drawing.
        */
        void draw(const Shader *shader);

        /**
        * @brief Destroys the mesh in the OpenGL context.
        */
        void destroy();

        /**
        * @brief Returns the name of the class.
        * @returns The name of the class.
        */
        std::string_view name() const {
            return "Mesh";
        }

    private:
        /**
        * @brief Constructs a Mesh object. Used internally by the @ref engine::resources::ResourcesController class. You are not supposed to call this constructor directly from user code.
        * @param id The OpenGL ID of the mesh.
        * @param num_indices The number of indices in the mesh.
        * @param textures The textures in the mesh.
        */      
        Mesh(uint32_t id, uint64_t num_indices, std::vector<Texture *> textures) : m_vao(id)
          , m_num_indices(num_indices)
          , m_textures(std::move(textures)) {
        }

        uint32_t m_vao         = 0;
        uint64_t m_num_indices = 0;
        std::vector<Texture *> m_textures;

        static std::string_view texture_type_to_uniform_name_convention(TextureType type);
    };
} // namespace engine

#endif//MATF_RG_PROJECT_MESH_HPP
