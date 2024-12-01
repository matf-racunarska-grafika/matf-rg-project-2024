
//

#ifndef MATF_RG_PROJECT_MODEL_HPP
#define MATF_RG_PROJECT_MODEL_HPP
#include <engine/util/Utils.hpp>
#include <engine/render/Mesh.hpp>
#include <algorithm>
namespace rg {

    class Model {
        friend class AssetsController;

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
        void attach_meshes(std::vector<Mesh> meshes) {
            std::move(range(meshes), std::back_inserter(m_meshes));
        }
        std::vector<Mesh> m_meshes;
    };

}// namespace rg

#endif//MATF_RG_PROJECT_MODEL_HPP
