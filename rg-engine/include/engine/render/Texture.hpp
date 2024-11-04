
//

#ifndef MATF_RG_PROJECT_TEXTURE_HPP
#define MATF_RG_PROJECT_TEXTURE_HPP

#include <engine/ecs/Component.hpp>

namespace rg {
    class Texture : public DrawableComponent {
        friend class AssetsController;

    public:
        void draw() override;
        void destroy();

        static std::string_view type_name() {
            return "Texture";
        }

        std::string_view name() const override {
            return type_name();
        }

    private:
        uint32_t m_id;
    };
}// namespace rg
#endif//MATF_RG_PROJECT_TEXTURE_HPP
