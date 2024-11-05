
//

#ifndef MATF_RG_PROJECT_MODEL_HPP
#define MATF_RG_PROJECT_MODEL_HPP
#include <engine/ecs/Component.hpp>
#include <engine/render/Mesh.hpp>
#include <engine/render/DrawableElement.hpp>
#include <algorithm>
namespace rg {

    class Model : public DrawableElement {
        friend class AssetsController;

    public:
        void initialize() override;

        void draw(ShaderProgram *shader) override;

        std::string_view name() const override {
            return "Model";
        }

        void destroy() override;

    private:
        void attach_meshes(std::vector<Mesh> meshes) {
            std::move(range(meshes), std::back_inserter(m_meshes));
        }
        std::vector<Mesh> m_meshes;
    };

}// namespace rg

#endif//MATF_RG_PROJECT_MODEL_HPP
