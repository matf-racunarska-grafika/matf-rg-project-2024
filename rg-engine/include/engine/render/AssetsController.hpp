
//

#ifndef MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
#define MATF_RG_PROJECT_ASSETSCONTROLLER_HPP

#include <engine/controller/Controller.hpp>
#include <engine/render/Model.hpp>
#include <engine/render/Texture.hpp>
#include <assimp/scene.h>

namespace rg {

    class AssetsController : public rg::Controller {
    public:
        static std::unique_ptr<AssetsController> create() {
            return std::make_unique<AssetsController>();
        }

        std::string_view name() const override {
            return "AssetsController";
        }

        Result<Model *, AssetLoadingError> model(const std::string &model_name);
        Texture *texture(const std::string &texture_name);

        Texture *load_texture_from_file(const std::filesystem::path &file_name, TextureType type);

    private:
        void initialize() override;
        void terminate() override;

        struct ModelData {
            std::filesystem::path path;
            std::string name;
            Model model;
        };

        struct TextureData {
            std::filesystem::path path;
            std::string name;
            Texture texture;
            TextureType texture_type;

            TextureData(std::filesystem::path path, std::string name, Texture texture, TextureType texture_type);
        };


        std::unique_ptr<ModelData> load_model(const std::string &model_name);

        std::unordered_map<std::string, std::unique_ptr<ModelData>> m_models;
        std::unordered_map<std::string, std::unique_ptr<TextureData>> m_textures;
        std::filesystem::path m_models_filesystem_path;
    };
}// namespace rg

#endif//MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
