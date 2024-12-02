
//

#ifndef MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
#define MATF_RG_PROJECT_ASSETSCONTROLLER_HPP

#include <engine/controller/Controller.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/Texture.hpp>
#include <utility>
#include <assimp/scene.h>

namespace rg {
    class ResourcesController : public Controller {
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

        Texture *load_from_file_if_absent(const std::filesystem::path &file_name,
                                          TextureType type = TextureType::Regular, bool flip_uvs = false);

    private:
        void load_models();

        void load_textures();

        void load_skyboxes();

        void initialize() override;

        void terminate() override;

        struct TextureData {
            TextureData(std::filesystem::path path, std::string name, Texture texture, TextureType texture_type);
        };

        std::unique_ptr<Model> load_model(const std::string &model_name);

        std::unique_ptr<Texture> load_texture(const std::filesystem::path &path,
                                              TextureType texture_type = TextureType::Regular,
                                              bool flip_uvs            = false);

        std::unordered_map<std::string, std::unique_ptr<Model> > m_models;
        std::unordered_map<std::string, std::unique_ptr<Texture> > m_textures;
        std::filesystem::path m_models_path;
        std::filesystem::path m_textures_path;
        std::filesystem::path m_shaders_path;
        std::filesystem::path m_skyboxes_path;
    };
} // namespace rg

#endif//MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
