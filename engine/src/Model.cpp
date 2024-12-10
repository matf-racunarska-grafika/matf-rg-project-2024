
#include <engine/resources/Model.hpp>

namespace engine::resources {

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
