
//

#ifndef MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
#define MATF_RG_PROJECT_ASSETSCONTROLLER_HPP

#include <engine/controller/Controller.hpp>
#include <engine/render/Model.hpp>
#include <engine/render/Texture.hpp>

namespace rg {

    class AssetsController : public rg::Controller {
    public:
        static std::unique_ptr<AssetsController> create() {
            return std::make_unique<AssetsController>();
        }

        std::string_view name() const override {
            return "AssetsController";
        }

    private:
        void initialize() override;
        void terminate() override;

    private:
        struct ModelData {
            std::filesystem::path path;
            std::string name;
            Model model;
        };

        enum class TextureType {
            Diffuse,
            Specular,
            Normal,
            Height,
        };

        struct TextureData {
            std::filesystem::path path;
            std::string name;
            Texture texture;
            TextureType texture_type;
        };

        std::unordered_map<std::filesystem::path, std::unique_ptr<ModelData>> m_models;
        std::unordered_map<std::filesystem::path, std::unique_ptr<Texture>> m_textures;
    };
}// namespace rg

#endif//MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
