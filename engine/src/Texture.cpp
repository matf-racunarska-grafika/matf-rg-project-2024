//
// Created by spaske on 12/2/24.
//
#include <glad/glad.h>
#include <engine/resources/Texture.hpp>
#include <engine/util/Utils.hpp>
#include <engine/util/Errors.hpp>
#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <format>

namespace rg {

    std::string_view texture_type_to_string(TextureType type) {
        switch (type) {
        case TextureType::Diffuse: return "Diffuse";
        case TextureType::Specular: return "Specular";
        case TextureType::Normal: return "Normal";
        case TextureType::Height: return "Height";
        case TextureType::CubeMap: return "CubeMap";
        default: RG_SHOULD_NOT_REACH_HERE("Unknown TextureType");
        }
    }

    Texture Texture::create_from_file(std::filesystem::path path, TextureType type) {
        if (type == TextureType::CubeMap) {
            return {load_cubemap_texture(path), type};
        }
        return {load_regular_texture(path), type};
    }

    uint32_t Texture::load_regular_texture(std::filesystem::path path) {
        uint32_t texture_id;
        glGenTextures(1, &texture_id);

        int32_t width, height, nr_components;
        uint8_t *data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);
        defer {
            stbi_image_free(data);
        };
        if (data) {
            GLenum format;
            if (nr_components == 1)
                format = GL_RED;
            else if (nr_components == 3)
                format = GL_RGB;
            else if (nr_components == 4)
                format = GL_RGBA;
            else
                throw AssetLoadingError("Unknown number of channels");

            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            throw AssetLoadingError(std::format("Failed to load texture {}", path.string()));
        }
        return texture_id;
    }

    uint32_t face_index(std::string_view name) {
        if (name == "right") {
            return 0;
        } else if (name == "left") {
            return 1;
        } else if (name == "top") {
            return 2;
        } else if (name == "bottom") {
            return 3;
        } else if (name == "front") {
            return 4;
        } else if (name == "back") {
            return 5;
        } else {
            RG_SHOULD_NOT_REACH_HERE(
                    "Unknown face name: {}. The cubemap textures should be named: right, left, top, bottom, front, back; by their respective faces in the cubemap. The extension of the image file is ignored.",
                    name);
        }
    }

    uint32_t Texture::load_cubemap_texture(std::filesystem::path path) {
        RG_GUARANTEE(std::filesystem::is_directory(path),
                     "Please specify path to be a directory to where the cubemap textures are located. The cubemap textures should be named: right, left, top, bottom, front, back; by their respective faces in the cubemap")
        ;
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (const auto &file: std::filesystem::directory_iterator(path)) {
            unsigned char *data = stbi_load(absolute(file).c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                uint32_t i = face_index(file.path().stem().c_str());
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                             data);
                stbi_image_free(data);
            } else {
                spdlog::error("Failed to load cubemap texture {}", file.path().string());
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    void Texture::destroy() {
        glDeleteTextures(1, &m_id);
    }

    std::string_view Texture::uniform_name_convention(TextureType type) {
        switch (type) {
        case TextureType::Diffuse: return "texture_diffuse";
        case TextureType::Specular: return "texture_specular";
        case TextureType::Normal: return "texture_normal";
        case TextureType::Height: return "texture_height";
        case TextureType::CubeMap: return "texture_cubemap";
        default: RG_SHOULD_NOT_REACH_HERE("Unhandled TextureType");
        }
    }

}
