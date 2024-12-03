#include <glad/glad.h>
#include <stb_image.h>
#include <engine/resources/ResourcesController.hpp>
#include <engine/util/Errors.hpp>
#include <engine/util/Utils.hpp>
#include <spdlog/spdlog.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_set>
#include <utility>
#include <engine/platform/OpenGL.hpp>

namespace rg {

    void ResourcesController::initialize() {
        load_models();
        load_textures();
        load_skyboxes();
    }

    void ResourcesController::load_models() {
        spdlog::info("load_models::begin");
        const auto &config = Configuration::config();
        m_models_path      = "resources/models/";
        if (exists(m_models_path)) {
            for (const auto &model_entry: config["resources"]["models"].items()) {
                load_model(model_entry.key());
            }
        }
        spdlog::info("load_models::end");
    }

    void ResourcesController::load_textures() {
        spdlog::info("load_textures::begin");
        m_textures_path = "resources/textures/";
        if (exists(m_textures_path)) {
            for (const auto &texture_entry: std::filesystem::directory_iterator(m_textures_path)) {
                // load_texture(texture_entry.path());
            }
        }
        spdlog::info("load_textures::end");
    }

    void ResourcesController::load_skyboxes() {
        spdlog::info("load_skyboxes::begin");
        m_skyboxes_path = "resources/skyboxes/";
        if (exists(m_skyboxes_path)) {
            for (const auto &sky_boxes_entry: std::filesystem::directory_iterator(m_skyboxes_path)) {
                load_skybox(sky_boxes_entry.path(), sky_boxes_entry.path().stem().string());
            }
        }
        spdlog::info("load_skyboxes::end");
    }

    void ResourcesController::terminate() {
    }

    Model *ResourcesController::model(const std::string &name) {
        auto &model_data = m_models[name];
        if (!model_data) {
            model_data = load_model(name);
        }
        return model_data.get();
    }

    Texture *ResourcesController::texture(const std::string &texture_name) {
        auto &config = Configuration::config();
        std::filesystem::path texture_path(m_textures_path);
        return load_from_file_if_absent(texture_path / texture_name, TextureType::Regular);
    }

    Skybox *ResourcesController::skybox(const std::string &name) {
        auto &result = m_sky_boxes[name];
        if (!result) {
            result = load_skybox(m_skyboxes_path / name, name);
        }
        return result.get();
    }

    struct SceneProcessingResult {
        std::vector<Mesh> meshes;
        std::unordered_set<Texture *> textures;
    };

    class AssimpSceneProcessor {
    public:
        using TextureLoadingCallback = std::function<Texture *(std::filesystem::path)>;

        static SceneProcessingResult process_scene(ResourcesController *assets_controller, const aiScene *scene,
                                                   std::filesystem::path model_path);

    private:
        explicit AssimpSceneProcessor(ResourcesController *assets_controller, const aiScene *scene,
                                      std::filesystem::path model_path) :
        m_model_path(std::move(model_path)), m_scene(scene), m_resources_controller(assets_controller) {
        }

        void process_node(aiNode *node);

        void process_mesh(aiMesh *mesh);

        void process_materials(aiMaterial *material);

        const aiScene *m_scene;
        std::vector<Mesh> m_meshes;
        std::unordered_set<Texture *> m_textures;

        void process_material_type(aiMaterial *material, aiTextureType type);

        ResourcesController *m_resources_controller;
        std::filesystem::path m_model_path;

        static TextureType assimp_texture_type_to_engine(aiTextureType type);
    };

    std::unique_ptr<rg::Model> ResourcesController::load_model(
            const std::string &model_name) {
        auto &config = Configuration::config();
        if (!config["resources"]["models"].contains(model_name)) {
            throw ConfigurationError(std::format(
                    "No model ({}) specify in config.json. Please add the model to the config.json.",
                    model_name));
        }
        std::filesystem::path model_path = m_models_path /
                                           std::filesystem::path(
                                                   config["resources"]["models"][model_name]["path"].get<
                                                       std::string>());

        Assimp::Importer importer;
        int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                    aiProcess_CalcTangentSpace;
        if (config["resources"]["models"][model_name].value<bool>("flip_uvs", false)) {
            flags |= aiProcess_FlipUVs;
        }

        spdlog::info("load_model({}, {})", model_name, model_path.string());
        const aiScene *scene =
                importer.ReadFile(model_path, flags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw AssetLoadingError("Assimp error while reading model. ", model_path, model_name);
        }

        SceneProcessingResult result = AssimpSceneProcessor::process_scene(this, scene, model_path);
        return std::make_unique<Model>(Model(std::move(result.meshes), model_path, model_name));
    }

    std::unique_ptr<Texture> ResourcesController::load_texture(
            const std::filesystem::path &path,
            TextureType type, bool flip_uvs) {
        spdlog::info("Loading texture: {}", path.string());
        return std::make_unique<Texture>(Texture(OpenGL::load_texture(path, flip_uvs), type, path, path.stem()));
    }

    std::unique_ptr<Skybox> ResourcesController::load_skybox(const std::filesystem::path &path,
                                                             const std::string &name,
                                                             bool flip_uvs) {
        spdlog::info("Loading skybox: {}", path.string());
        return std::make_unique<Skybox>(Skybox(OpenGL::init_skybox_vao(), OpenGL::load_skybox_textures(path, flip_uvs),
                                               path, name));
    }

    SceneProcessingResult AssimpSceneProcessor::process_scene(ResourcesController *assets_controller,
                                                              const aiScene *scene,
                                                              std::filesystem::path model_path) {
        AssimpSceneProcessor scene_processor(assets_controller, scene, std::move(model_path));
        scene_processor.process_node(scene->mRootNode);
        SceneProcessingResult scene_processing_result;
        scene_processing_result.meshes   = std::move(scene_processor.m_meshes);
        scene_processing_result.textures = std::move(scene_processor.m_textures);
        return scene_processing_result;
    }

    void AssimpSceneProcessor::process_node(aiNode *node) {
        for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
            auto mesh = m_scene->mMeshes[node->mMeshes[i]];
            process_mesh(mesh);
        }
        for (uint32_t i = 0; i < node->mNumChildren; ++i) {
            process_node(node->mChildren[i]);
        }
    }

    void AssimpSceneProcessor::process_mesh(aiMesh *mesh) {
        std::vector<Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            rg::Vertex vertex{};
            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;

            if (mesh->HasNormals()) {
                vertex.Normal.x = mesh->mNormals[i].x;
                vertex.Normal.y = mesh->mNormals[i].y;
                vertex.Normal.z = mesh->mNormals[i].z;
            }

            if (mesh->mTextureCoords[0]) {
                vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
                vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;

                vertex.Tangent.x = mesh->mTangents[i].x;
                vertex.Tangent.y = mesh->mTangents[i].y;
                vertex.Tangent.z = mesh->mTangents[i].z;

                vertex.Bitangent.x = mesh->mBitangents[i].x;
                vertex.Bitangent.y = mesh->mBitangents[i].y;
                vertex.Bitangent.z = mesh->mBitangents[i].z;
            }
            vertices.push_back(vertex);
        }

        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];

            for (uint32_t j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }

        auto material = m_scene->mMaterials[mesh->mMaterialIndex];
        process_materials(material);
        m_meshes.emplace_back(Mesh::create(vertices, indices, m_textures));
    }

    void AssimpSceneProcessor::process_materials(aiMaterial *material) {
        auto ai_texture_types = {
                aiTextureType_DIFFUSE,
                aiTextureType_SPECULAR,
                aiTextureType_NORMALS,
                aiTextureType_HEIGHT,
        };

        for (auto ai_texture_type: ai_texture_types) {
            process_material_type(material, ai_texture_type);
        }
    }

    void AssimpSceneProcessor::process_material_type(aiMaterial *material, aiTextureType type) {
        auto material_count = material->GetTextureCount(type);
        for (uint32_t i = 0; i < material_count; ++i) {
            aiString ai_texture_path_string;
            material->GetTexture(type, i, &ai_texture_path_string);
            std::filesystem::path texture_path(ai_texture_path_string.C_Str());
            Texture *texture = m_resources_controller->load_from_file_if_absent(
                    m_model_path.parent_path() / texture_path,
                    assimp_texture_type_to_engine(type));
            m_textures.emplace(texture);
        }
    }

    TextureType AssimpSceneProcessor::assimp_texture_type_to_engine(aiTextureType type) {
        switch (type) {
        case aiTextureType_DIFFUSE: return TextureType::Diffuse;
        case aiTextureType_SPECULAR: return TextureType::Specular;
        case aiTextureType_HEIGHT: return TextureType::Height;
        case aiTextureType_NORMALS: return TextureType::Normal;
        default: RG_SHOULD_NOT_REACH_HERE("Engine currently doesn't support the aiTextureType: {}",
                                          static_cast<int>(type));
        }
    }

    Texture *ResourcesController::load_from_file_if_absent(
            const std::filesystem::path &path, TextureType type,
            bool flip_uvs) {
        auto &result = m_textures[path];
        if (!result) {
            result = load_texture(path, type, flip_uvs);
        }
        return result.get();
    }

} // namespace rg
