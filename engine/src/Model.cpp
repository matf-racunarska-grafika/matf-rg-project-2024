
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::resources {

    void Model::draw(const Shader *shader) {
        shader->use();
        for (auto &mesh: m_meshes) {
            mesh.draw(shader);
        }
    }

    int Model::initialization_instances(const std::vector<glm::mat4> &instances) {
        int ret = 0;
        for (auto &mesh: m_meshes) {
            if (ret != 0) {
                throw std::runtime_error("failed to initialize meshes");
            }
            ret = mesh.initialization_mesh_instances(instances);
        }
        return ret;
    }

    int Model::draw_instanced(const Shader *shader, const int size) {
        shader->use();
        int ret = 0;
        for (auto &mesh : m_meshes) {
            if (ret != 0) {
                throw std::runtime_error("failed to draw instanced meshes");
            }
            ret = mesh.draw(shader, size);
        }
        return ret;
    }

    void Model::destroy() {
        for (auto &mesh: m_meshes) {
            mesh.destroy();
        }
    }
}
