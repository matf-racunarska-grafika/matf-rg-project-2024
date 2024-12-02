//
// Created by spaske on 12/2/24.
//

#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace rg {

    void Model::draw(ShaderProgram *shader) {
        for (auto &mesh: m_meshes) {
            mesh.draw(shader);
        }
    }

    void Model::destroy() {
        for (auto &mesh: m_meshes) {
            mesh.destroy();
        }
    }

    void Model::initialize() {
    }

}
