#include <engine/resources/ResourcesController.hpp>
#include <engine/util/Errors.hpp>
#include <engine/util/Utils.hpp>
#include <spdlog/spdlog.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_set>
#include <utility>

namespace rg {

    void ResourcesController::initialize() {
        load_models();
        load_textures();
        load_shaders();
    }

    void ResourcesController::load_models() {
        spdlog::info("load_models::begin");
        const auto &config = Configuration::config();
        m_models_path      = config["resources"]["models_path"].get<std::string>();
        for (const auto &model_entry: config["resources"]["models"].items()) {
            load_model(model_entry.key());
        }
        spdlog::info("load_models::end");
    }


    void ResourcesController::terminate() {
    }

    Model *ResourcesController::model(const std::string &model_name) {
        auto &model_data = m_models[model_name];
        if (!model_data) {
            model_data = load_model(model_name);
        }
        return &model_data->model;
    }

    Texture *ResourcesController::texture(const std::string &texture_name) {
        auto &config = Configuration::config();
        std::filesystem::path texture_path(config["resources"]["textures_path"].get<std::string>());
        return load_from_file_if_absent(texture_path / texture_name, TextureType::Standalone);
    }

    Texture *ResourcesController::skybox(const std::string &texture_name) {
        auto &config = Configuration::config();
        std::filesystem::path texture_path(config["resources"]["textures_path"].get<std::string>());
        return load_from_file_if_absent(texture_path / texture_name, TextureType::CubeMap);
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
        m_model_path(std::move(model_path)), m_scene(scene), m_assets_controller(assets_controller) {
        }

        void process_node(aiNode *node);

        void process_mesh(aiMesh *mesh);

        void process_materials(aiMaterial *material);

        const aiScene *m_scene;
        std::vector<Mesh> m_meshes;
        std::unordered_set<Texture *> m_textures;

        void process_material_type(aiMaterial *material, aiTextureType type);

        ResourcesController *m_assets_controller;
        std::filesystem::path m_model_path;

        static TextureType assimp_texture_type_to_engine(aiTextureType type);
    };

    std::unique_ptr<ResourcesController::ModelData> ResourcesController::load_model(const std::string &model_name) {
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
        return std::make_unique<ModelData>(model_path, model_name, Model(std::move(result.meshes)));
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
            Texture *texture = m_assets_controller->load_from_file_if_absent(m_model_path.parent_path() / texture_path,
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

    Texture *ResourcesController::load_from_file_if_absent(const std::filesystem::path &path, TextureType type) {
        auto &texture_data = m_textures[path];
        if (!texture_data) {
            spdlog::info("Loading texture: {}", path.string());
            texture_data = std::make_unique<TextureData>(path, path.string(), Texture::create_from_file(path, type),
                                                         type);
        }
        return &texture_data->texture;
    }

    ResourcesController::TextureData::TextureData(std::filesystem::path path, std::string name, Texture texture,
                                                  TextureType texture_type) :
    path(std::move(path)), name(std::move(name)), texture(std::move(texture)), texture_type(texture_type) {
    }
} // namespace rg
