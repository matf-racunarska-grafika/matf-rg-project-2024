//
// Created by spaske on 12/3/24.
//
#include <glad/glad.h>
#include <filesystem>
#include <array>
#include <stb_image.h>
#include <engine/platform/OpenGL.hpp>
#include <engine/util/Errors.hpp>
#include <engine/util/Utils.hpp>

namespace rg {

    uint32_t stbi_number_of_channels_to_gl_format(int32_t number_of_channels);

    bool OpenGL::initialize(GLADloadproc loader) {
        return gladLoadGLLoader(loader);
    }

    uint32_t OpenGL::load_texture(std::filesystem::path path, bool flip_uvs) {
        uint32_t texture_id;
        glGenTextures(1, &texture_id);

        int32_t width, height, nr_components;
        stbi_set_flip_vertically_on_load(flip_uvs);
        uint8_t *data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);
        defer {
            stbi_image_free(data);
        };
        if (data) {
            GLenum format = stbi_number_of_channels_to_gl_format(nr_components);

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

    uint32_t OpenGL::init_skybox_vao() {
        static unsigned int skybox_vao = 0;
        if (skybox_vao != 0) {
            return skybox_vao;
        }
        float vertices[] = {
#include <skybox_vertices.include>
        };
        uint32_t skybox_vbo = 0;
        glGenVertexArrays(1, &skybox_vao);
        glGenBuffers(1, &skybox_vbo);
        glBindVertexArray(skybox_vao);
        glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
        return skybox_vao;
    }

    uint32_t face_index(std::string_view name);

    uint32_t OpenGL::load_skybox_textures(std::filesystem::path path, bool flip_uvs) {
        RG_GUARANTEE(std::filesystem::is_directory(path),
                     "Please specify path to be a directory to where the cubemap textures are located. The cubemap textures should be named: right, left, top, bottom, front, back; by their respective faces in the cubemap")
        ;
        uint32_t texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

        int width, height, nr_channels;
        for (const auto &file: std::filesystem::directory_iterator(path)) {
            stbi_set_flip_vertically_on_load(flip_uvs);
            unsigned char *data = stbi_load(absolute(file).c_str(), &width, &height, &nr_channels, 0);
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

    uint32_t stbi_number_of_channels_to_gl_format(int32_t number_of_channels) {
        switch (number_of_channels) {
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: RG_SHOULD_NOT_REACH_HERE("Unknown channels {}", number_of_channels);
        }
    }

};
