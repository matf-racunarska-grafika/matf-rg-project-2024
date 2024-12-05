
//

#ifndef MATF_RG_PROJECT_RESOURCES_CONTROLLER_HPP
#define MATF_RG_PROJECT_RESOURCES_CONTROLLER_HPP

#define MATF_RG_PROJECT_RESOURCES_CONTROLLER_HPP

#include <engine/controller/Controller.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/Texture.hpp>
#include <engine/resources/Shader.hpp>
#include <engine/resources/Skybox.hpp>

namespace rg {
    class ResourcesController final : public Controller {
    public:
        std::string_view name() const override {
            return "ResourcesController";
        }

        Model *model(const std::string &model_name);

        Texture *texture(const std::string &name,
                         const std::filesystem::path &path = "",
                         TextureType texture_type          = TextureType::Regular,
                         bool flip_uvs                     = false);

        Skybox *skybox(const std::string &name,
                       const std::filesystem::path &path = ""
                     , bool flip_uvs                     = false);

        Shader *shader(const std::string &name, const std::filesystem::path &path = "");

    private:
        void load_models();

        void load_textures();

        void load_skyboxes();

        void load_shaders();

        void initialize() override;

        std::unordered_map<std::string, std::unique_ptr<Model> > m_models;
        std::unordered_map<std::string, std::unique_ptr<Texture> > m_textures;
        std::unordered_map<std::string, std::unique_ptr<Skybox> > m_sky_boxes;
        std::unordered_map<std::string, std::unique_ptr<Shader> > m_shaders;

        const std::filesystem::path m_models_path   = "resources/models";
        const std::filesystem::path m_textures_path = "resources/textures";
        const std::filesystem::path m_shaders_path  = "resources/shaders";
        const std::filesystem::path m_skyboxes_path = "resources/skyboxes";
    };
} // namespace rg

#endif//MATF_RG_PROJECT_ASSETSCONTROLLER_HPP
