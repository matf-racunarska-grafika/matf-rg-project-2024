
//

#ifndef MATF_RG_PROJECT_TEXTURE_HPP
#define MATF_RG_PROJECT_TEXTURE_HPP

#include <string_view>
#include <filesystem>
#include <utility>

namespace rg {
    class Shader;

    enum class TextureType {
        Regular,
        Diffuse,
        Specular,
        Normal,
        Height,
    };

    class Texture {
        friend class ResourcesController;

    public:
        static std::string_view uniform_name_convention(TextureType type);

        void destroy();

        TextureType type() const {
            return m_type;
        }

        uint32_t id() const {
            return m_id;
        }

        const std::filesystem::path &path() const {
            return m_path;
        }

        const std::string &name() const {
            return m_name;
        }

        Texture() = default;

    private:
        Texture(uint32_t id, TextureType type, std::filesystem::path path, std::string name)
        : m_id(id)
      , m_type(type)
      , m_path(std::move(path))
      , m_name(std::move(name)) {
        }

        uint32_t m_id{};
        TextureType m_type{};
        std::filesystem::path m_path{};
        std::string m_name{};
    };
} // namespace rg
#endif//MATF_RG_PROJECT_TEXTURE_HPP
