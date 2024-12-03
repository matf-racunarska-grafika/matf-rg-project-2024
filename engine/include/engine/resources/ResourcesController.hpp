
//

#ifndef MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
#define MATF_RG_PROJECT_ASSETSCONTROLLER_HPP

#include <engine/controller/Controller.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/Texture.hpp>
#include <engine/resources/Skybox.hpp>
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

        Texture *texture(const std::string &name,
                         std::filesystem::path path = "",
                         TextureType texture_type   = TextureType::Regular,
                         bool flip_uvs              = false);

        Skybox *skybox(const std::string &name,
                       std::filesystem::path path = ""
                     , bool flip_uvs              = false);

    private:
        void load_models();

        void load_textures();

        void load_skyboxes();

        void initialize() override;

        void terminate() override;

        std::unordered_map<std::string, std::unique_ptr<Model> > m_models;
        std::unordered_map<std::string, std::unique_ptr<Texture> > m_textures;
        std::unordered_map<std::string, std::unique_ptr<Skybox> > m_sky_boxes;

        const std::filesystem::path m_models_path   = "resources/models";
        const std::filesystem::path m_textures_path = "resources/textures";
        const std::filesystem::path m_shaders_path  = "resources/shaders";
        const std::filesystem::path m_skyboxes_path = "resources/skyboxes";
    };
} // namespace rg

#endif//MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
