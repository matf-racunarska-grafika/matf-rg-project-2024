//
// Created by spaske on 12/3/24.
//

#ifndef OPENGL_HPP
#define OPENGL_HPP
#include <cstdint>
#include <filesystem>
#include <engine/resources/Shader.hpp>

namespace rg {
    class Skybox;

    class OpenGL {
    public:
        using ShaderProgramId = uint32_t;

        static bool initialize(void *(*loader)(const char *name));

        static uint32_t initialize_texture(std::filesystem::path path, bool flip_uvs = false);

        static uint32_t load_texture(const std::filesystem::path &path, bool flip_uvs);

        static int32_t texture_format(int32_t number_of_channels);

        static uint32_t init_skybox_vao();

        static bool shader_compiled_successfully(uint32_t shader_id);

        static bool compile_shader(uint32_t shader_id, const std::string &shader_source);

        static uint32_t load_skybox_textures(const std::filesystem::path &path, bool flip_uvs = false);

        static void enable_depth_testing();

        static void clear_buffers();

        static void draw_skybox(const Skybox *skybox);

        static void terminate();

        static std::string get_compilation_error_message(uint32_t shader_id);
    };
}
#endif //OPENGL_HPP
