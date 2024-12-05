#include <engine/resources/Shader.hpp>
#include <engine/resources/ShaderCompiler.hpp>
#include <engine/resources/Resources.hpp>
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
        load_shaders();
        load_models();
        load_textures();
        load_skyboxes();
    }

    void ResourcesController::load_shaders() {
        for (const auto &shader_path: std::filesystem::directory_iterator(m_shaders_path)) {
            const auto name = shader_path.path().stem().string();
            shader(name, shader_path);
        }
    }

    void ResourcesController::load_models() {
        const auto &config = Configuration::config();
        if (exists(m_models_path)) {
            for (const auto &model_entry: config["resources"]["models"].items()) {
                model(model_entry.key());
            }
        }
    }

    void ResourcesController::load_textures() {
        if (exists(m_textures_path)) {
            for (const auto &texture_entry: std::filesystem::directory_iterator(m_textures_path)) {
                texture(texture_entry.path().stem().string(), texture_entry.path());
            }
        }
    }

    void ResourcesController::load_skyboxes() {
        if (exists(m_skyboxes_path)) {
            for (const auto &sky_boxes_entry: std::filesystem::directory_iterator(m_skyboxes_path)) {
                skybox(sky_boxes_entry.path().stem().string(), sky_boxes_entry.path());
            }
        }
    }

    class AssimpSceneProcessor {
    public:
        using TextureLoadingCallback = std::function<Texture *(std::filesystem::path)>;

        static std::vector<Mesh> process_scene(ResourcesController *resources_controller, const aiScene *scene,
                                               std::filesystem::path model_path);

    private:
        explicit AssimpSceneProcessor(ResourcesController *resources_controller, const aiScene *scene,
                                      std::filesystem::path model_path) :
        m_resources_controller(resources_controller), m_model_path(std::move(model_path)), m_scene(scene) {
        }

        void process_node(aiNode *node);

        void process_mesh(aiMesh *mesh);

        std::vector<Texture *> process_materials(aiMaterial *material);

        void process_material_type(std::vector<Texture *> &textures, aiMaterial *material, aiTextureType type);

        static TextureType assimp_texture_type_to_engine(aiTextureType type);

        std::vector<Mesh> m_meshes;
        const aiScene *m_scene;
        std::filesystem::path m_model_path;
        ResourcesController *m_resources_controller;
    };

    Model *ResourcesController::model(
            const std::string &model_name) {
        auto &result = m_models[model_name];
        if (!result) {
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

            std::vector<Mesh> meshes = AssimpSceneProcessor::process_scene(this, scene, model_path);
            result                   = std::make_unique<Model>(Model(std::move(meshes), model_path,
                                                   model_name));
        }
        return result.get();
    }

    Texture *ResourcesController::texture(const std::string &name,
                                          std::filesystem::path path,
                                          TextureType type, bool flip_uvs) {
        auto &result = m_textures[name];
        if (!result) {
            spdlog::info("Loading texture: {}", path.string());
            result = std::make_unique<Texture>(Texture(OpenGL::load_texture(path, flip_uvs), type, path, path.stem()));
        }
        return result.get();
    }

    Skybox *ResourcesController::skybox(const std::string &name,
                                        std::filesystem::path path,
                                        bool flip_uvs) {
        auto &result = m_sky_boxes[name];
        if (!result) {
            spdlog::info("Loading skybox: {}", path.string());
            result = std::make_unique<Skybox>(Skybox(OpenGL::init_skybox_vao(),
                                                     OpenGL::load_skybox_textures(path, flip_uvs),
                                                     path, name));
        }
        return result.get();
    }

    Shader *ResourcesController::shader(const std::string &name, std::filesystem::path path) {
        auto &result = m_shaders[name];
        if (!result) {
            result = std::make_unique<Shader>(ShaderCompiler::compile_from_file(name, path));
        }
        return result.get();
    }

    std::vector<Mesh> AssimpSceneProcessor::process_scene(ResourcesController *resources_controller,
                                                          const aiScene *scene,
                                                          std::filesystem::path model_path) {
        AssimpSceneProcessor scene_processor(resources_controller, scene, std::move(model_path));
        scene_processor.process_node(scene->mRootNode);
        return std::move(scene_processor.m_meshes);
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
            Vertex vertex{};
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

        auto material                   = m_scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture *> textures = process_materials(material);
        m_meshes.emplace_back(Mesh::create(vertices, indices, std::move(textures)));
    }

    std::vector<Texture *> AssimpSceneProcessor::process_materials(aiMaterial *material) {
        std::vector<Texture *> textures;
        auto ai_texture_types = {
                aiTextureType_DIFFUSE,
                aiTextureType_SPECULAR,
                aiTextureType_NORMALS,
                aiTextureType_HEIGHT,
        };

        for (auto ai_texture_type: ai_texture_types) {
            process_material_type(textures, material, ai_texture_type);
        }
        return textures;
    }

    void AssimpSceneProcessor::process_material_type(std::vector<Texture *> &textures, aiMaterial *material,
                                                     aiTextureType type) {
        auto material_count = material->GetTextureCount(type);
        for (uint32_t i = 0; i < material_count; ++i) {
            aiString ai_texture_path_string;
            material->GetTexture(type, i, &ai_texture_path_string);
            std::filesystem::path texture_path = m_model_path.parent_path() / ai_texture_path_string.C_Str();
            Texture *texture                   = m_resources_controller->texture(texture_path.string(), texture_path,
                                                               assimp_texture_type_to_engine(type));
            textures.emplace_back(texture);
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

} // namespace rg
