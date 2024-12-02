
//

#ifndef MATF_RG_PROJECT_TEXTURE_HPP
#define MATF_RG_PROJECT_TEXTURE_HPP

#include <string_view>
#include <filesystem>

namespace rg {
    class ShaderProgram;

    enum class TextureType {
        Standalone,
        Diffuse,
        Specular,
        Normal,
        Height,
        CubeMap,
    };

    std::string_view texture_type_to_string(TextureType type);

    class Texture {
        friend class AssetsController;

    public:
        static Texture create_from_file(std::filesystem::path path, TextureType type);

        void destroy();

        TextureType type() const {
            return m_type;
        }

        uint32_t id() const {
            return m_id;
        }

    private:
        Texture(uint32_t id, TextureType type) : m_id(id)
                                             , m_type(type) {
        }

        static uint32_t load_regular_texture(std::filesystem::path path);

        static uint32_t load_cubemap_texture(std::filesystem::path path);

        uint32_t m_id;
        TextureType m_type;
    };
} // namespace rg
#endif//MATF_RG_PROJECT_TEXTURE_HPP
