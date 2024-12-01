
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

    std::string_view texture_type_to_string(TextureType type);

    class Texture : public DrawableElement {
        Texture(uint32_t id, TextureType type) : m_id(id), m_type(type) {
        }
        friend class AssetsController;
    public:
        static Texture create_from_file(std::filesystem::path path, TextureType type);

        void initialize() override;

        void draw(ShaderProgram *shader) override;

        void destroy() override;

        static std::string_view type_name() {
            return "Texture";
        }

        std::string_view name() const override {
            return type_name();
        }

        TextureType type() const { return m_type; }

        uint32_t id() const { return m_id; }
    private:
        uint32_t m_id;
        TextureType m_type;
    };

}// namespace rg
#endif//MATF_RG_PROJECT_TEXTURE_HPP
