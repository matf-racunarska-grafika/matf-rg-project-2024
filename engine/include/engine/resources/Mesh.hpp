/**
 * @file Mesh.hpp
 * @brief Defines the Mesh class that serves as the interface for mesh rendering and storing processed assimp scenes.
 */

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
    friend class AssimpSceneProcessor;

public:

    /**
    * @brief Draws the mesh using a given shader. Called by the @ref Model::draw function to draw all the meshes in the model.
    * @param shader The shader to use for drawing.
    */
    void draw(const Shader *shader);

    /**
    * @brief Destroys the mesh in the OpenGL context.
    */
    void destroy();

    uint32_t get_vao() const {return m_vao;}

    void draw_instanced(const engine::resources::Shader *shader,  unsigned int instances_amount);
private:
    /**
    * @brief Constructs a Mesh object.
    * @param vertices The vertices in the mesh.
    * @param indices The indices in the mesh.
    * @param textures The textures in the mesh.
     */
    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
         std::vector<Texture *> textures);

    uint32_t m_vao{0};
    uint32_t m_num_indices{0};
    std::vector<Texture *> m_textures;
};
} // namespace engine

#endif//MATF_RG_PROJECT_MESH_HPP
