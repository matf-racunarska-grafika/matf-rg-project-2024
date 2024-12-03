//
// Created by spaske on 12/3/24.
//

#ifndef OPENGL_HPP
#define OPENGL_HPP
#include <cstdint>
#include <filesystem>

namespace rg {
    class Skybox;

    class OpenGL {
    public:
        static bool initialize(void *(*loader)(const char *name));

        static uint32_t initialize_texture(std::filesystem::path path, bool flip_uvs = false);

        static uint32_t load_texture(std::filesystem::path path, bool flip_uvs);

        static uint32_t init_skybox_vao();

        static uint32_t load_skybox_textures(std::filesystem::path path, bool flip_uvs = false);

        static void enable_depth_testing();

        static void clear_buffers();

        static void draw_skybox(const Skybox *skybox);

        static void terminate();
    };
}
#endif //OPENGL_HPP
