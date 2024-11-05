
//

#ifndef MATF_RG_PROJECT_TEXTURE_HPP
#define MATF_RG_PROJECT_TEXTURE_HPP

#include <engine/ecs/Component.hpp>
#include <engine/render/DrawableElement.hpp>
#include <string_view>
namespace rg {
    class ShaderProgram;

    enum class TextureType {
        Diffuse,
        Specular,
        Normal,
        Height,
    };


    class Texture : public DrawableElement {
        Texture(uint32_t id) : m_id(id) {
        }
        friend class AssetsController;
    public:
        static Texture create_from_file(std::filesystem::path path);

        void initialize() override;

        void draw(ShaderProgram *shader) override;

        void destroy() override;

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
