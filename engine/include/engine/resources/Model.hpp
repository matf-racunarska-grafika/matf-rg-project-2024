
//

#ifndef MATF_RG_PROJECT_MODEL_HPP
#define MATF_RG_PROJECT_MODEL_HPP
#include <engine/util/Utils.hpp>
#include <engine/resources/Mesh.hpp>
#include <algorithm>

namespace rg {
    class Model {
        friend class ResourcesController;

    public:
        void initialize();

        void draw(ShaderProgram *shader);

        std::string_view name() const {
            return "Model";
        }

        static std::string_view type_name() {
            return "Model";
        }

        void destroy();

    private:
        std::vector<Mesh> m_meshes;

        Model() = default;

        explicit Model(std::vector<Mesh> m_meshes) : m_meshes(std::move(m_meshes)) {
        }
    };
} // namespace rg

#endif//MATF_RG_PROJECT_MODEL_HPP
