//
// Created by spaske on 12/2/24.
//

#include <engine/resources/Model.hpp>

namespace rg {

    void Model::draw(const Shader *shader) {
        for (auto &mesh: m_meshes) {
            mesh.draw(shader);
        }
    }

    void Model::destroy() {
        for (auto &mesh: m_meshes) {
            mesh.destroy();
        }
    }
}
