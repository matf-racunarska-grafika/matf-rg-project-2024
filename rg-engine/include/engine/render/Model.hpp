
//

#ifndef MATF_RG_PROJECT_MODEL_HPP
#define MATF_RG_PROJECT_MODEL_HPP
#include <engine/ecs/Component.hpp>
#include <engine/render/Mesh.hpp>
namespace rg {

    class Model : public DrawableComponent {
        friend class AsssetsController;

    public:
        void draw() override;
        void destroy();

    private:
        std::vector<Mesh> m_meshes;
    };

}// namespace rg

#endif//MATF_RG_PROJECT_MODEL_HPP
