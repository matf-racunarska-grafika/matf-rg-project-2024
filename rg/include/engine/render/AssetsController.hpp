
//

#ifndef MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
#define MATF_RG_PROJECT_ASSETSCONTROLLER_HPP

#include <engine/controller/Controller.hpp>
#include <engine/render/Model.hpp>
#include <engine/render/Texture.hpp>
#include <assimp/scene.h>

namespace rg {
    class ResourcesController : public rg::Controller {
    public:
        static std::unique_ptr<ResourcesController> create() {
            return std::make_unique<ResourcesController>();
        }

        std::string_view name() const override {
            return "AssetsController";
        }

        Model *model(const std::string &model_name);

        Texture *texture(const std::string &name);

        Texture *skybox(const std::string &texture_name);

        Texture *load_from_file_if_absent(const std::filesystem::path &file_name, TextureType type);

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

        std::unique_ptr<TextureData> load_texture(const std::string &texture_name, TextureType texture_type);

        std::unordered_map<std::string, std::unique_ptr<ModelData> > m_models;
        std::unordered_map<std::string, std::unique_ptr<TextureData> > m_textures;
        std::filesystem::path m_models_filesystem_path;
    };
} // namespace rg

#endif//MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
