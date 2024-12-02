#include <engine/render/AssetsController.hpp>
#include <engine/util/Errors.hpp>
#include <engine/util/Utils.hpp>
#include <spdlog/spdlog.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_set>
#include <utility>
namespace rg {

    void AssetsController::initialize() {
        const auto &config = Configuration::config();
        m_models_filesystem_path = config["assets"]["models_path"].get<std::string>();
    }

    void AssetsController::terminate() {
    }

    Model *AssetsController::model(const std::string &model_name) {
        auto &model_data = m_models[model_name];
        if (!model_data) {
            model_data = load_model(model_name);
        }
        return &model_data->model;
    }

    Texture *AssetsController::texture(const std::string &texture_name) {
        auto &config = Configuration::config();
        std::filesystem::path texture_path(config["assets"]["textures_path"].get<std::string>());
        return load_from_file_if_absent(texture_path / texture_name, TextureType::Standalone);
    }

    Texture *AssetsController::skybox(const std::string &texture_name) {
        auto &config = Configuration::config();
        std::filesystem::path texture_path(config["assets"]["textures_path"].get<std::string>());
        return load_from_file_if_absent(texture_path / texture_name, TextureType::CubeMap);
    }

    struct SceneProcessingResult {
        std::vector<Mesh> meshes;
        std::unordered_set<Texture *> textures;
    };

    class AssimpSceneProcessor {
    public:
        using TextureLoadingCallback = std::function<Texture *(std::filesystem::path)>;
        static SceneProcessingResult process_scene(AssetsController *assets_controller, const aiScene *scene,
                                                   std::filesystem::path model_path);

    private:
        explicit AssimpSceneProcessor(AssetsController *assets_controller, const aiScene *scene,
                                      std::filesystem::path model_path)
            : m_model_path(std::move(model_path)), m_scene(scene), m_assets_controller(assets_controller) {
        }

        void process_node(aiNode *node);
        void process_mesh(aiMesh *mesh);
        void process_materials(aiMaterial *material);

        const aiScene *m_scene;
        std::vector<Mesh> m_meshes;
        std::unordered_set<Texture *> m_textures;
        void process_material_type(aiMaterial *material, aiTextureType type);
        AssetsController *m_assets_controller;
        std::filesystem::path m_model_path;
        static TextureType assimp_texture_type_to_engine(aiTextureType type);
    };

    std::unique_ptr<rg::AssetsController::ModelData> AssetsController::load_model(const std::string &model_name) {
        auto &config = Configuration::config();
        auto model_data = std::make_unique<ModelData>();
        model_data->name = model_name;
        model_data->path = m_models_filesystem_path /
                           std::filesystem::path(config["assets"]["models"][model_name]["path"].get<std::string>());
        spdlog::info("load_model({}, {})", model_data->name, model_data->path.string());

        Assimp::Importer importer;
        const aiScene *scene =
                importer.ReadFile(model_data->path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                            aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw AssetLoadingError("Assimp error while reading model. ", model_data->path, model_data->name);
        }

        SceneProcessingResult result = AssimpSceneProcessor::process_scene(this, scene, model_data->path);
        model_data->model.attach_meshes(std::move(result.meshes));
        return model_data;
    }

    SceneProcessingResult AssimpSceneProcessor::process_scene(AssetsController *assets_controller, const aiScene *scene,
                                                              std::filesystem::path model_path) {
        AssimpSceneProcessor scene_processor(assets_controller, scene, std::move(model_path));
        scene_processor.process_node(scene->mRootNode);
        SceneProcessingResult scene_processing_result;
        scene_processing_result.meshes = std::move(scene_processor.m_meshes);
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
            vertex.position.x = mesh->mVertices[i].x;
            vertex.position.y = mesh->mVertices[i].y;
            vertex.position.z = mesh->mVertices[i].z;

            if (mesh->HasNormals()) {
                vertex.normal.x = mesh->mNormals[i].x;
                vertex.normal.y = mesh->mNormals[i].y;
                vertex.normal.z = mesh->mNormals[i].z;
            }

            if (mesh->mTextureCoords[0]) {
                vertex.tex_cords.x = mesh->mTextureCoords[0][i].x;
                vertex.tex_cords.y = mesh->mTextureCoords[0][i].y;

                vertex.tangent.x = mesh->mTangents[i].x;
                vertex.tangent.y = mesh->mTangents[i].y;
                vertex.tangent.z = mesh->mTangents[i].z;

                vertex.bitangent.x = mesh->mBitangents[i].x;
                vertex.bitangent.y = mesh->mBitangents[i].y;
                vertex.bitangent.z = mesh->mBitangents[i].z;
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
        default: RG_SHOULD_NOT_REACH_HERE("Engine currently doesn't support the aiTextureType: {}", (int) type);
        }
    }

    Texture *AssetsController::load_from_file_if_absent(const std::filesystem::path &path, TextureType type) {
        auto &texture_data = m_textures[path];
        if (!texture_data) {
            spdlog::info("Loading texture: {}", path.string());
            texture_data = std::make_unique<TextureData>(path, path.string(), Texture::create_from_file(path, type), type);
        }
        return &texture_data->texture;
    }

    AssetsController::TextureData::TextureData(std::filesystem::path path, std::string name, Texture texture,
                                               TextureType texture_type)
        : path(std::move(path)), name(std::move(name)), texture(std::move(texture)), texture_type(texture_type) {
    }
}// namespace rg