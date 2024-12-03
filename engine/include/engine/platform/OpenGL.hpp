//
// Created by spaske on 12/3/24.
//

#ifndef OPENGL_HPP
#define OPENGL_HPP
#include <cstdint>
#include <filesystem>

namespace rg {
    class OpenGL {
    public:
        static bool initialize(GLADloadproc loader);

        static uint32_t initialize_texture(std::filesystem::path path, bool flip_uvs = false);

        static uint32_t load_texture(std::filesystem::path path, bool flip_uvs);

        static uint32_t init_skybox_vao();

        static uint32_t load_skybox_textures(std::filesystem::path path, bool flip_uvs = false);

        static void terminate();
    };
}
#endif //OPENGL_HPP
