//
// Created by spaske on 12/3/24.
//

#ifndef OPENGL_HPP
#define OPENGL_HPP
#include <cstdint>
#include <filesystem>
#include <engine/resources/Shader.hpp>

// TODO(mspasic): Add GL_ASSERT(...)

namespace engine::resources {
    class Skybox;
}

namespace engine::graphics {
    /**
    * @class OpenGL
    * @brief
    */
    class OpenGL {
    public:
        using ShaderProgramId = uint32_t;

        static int32_t shader_type_to_opengl_type(resources::ShaderType type);

        /**
        * @brief Loads the texture from `path` into the OpenGL context.
        *
        * @param path path to a texture file.
        * @param flip_uvs flip_uvs on load.
        * @returns OpenGL id of a texture object.
        */
        static uint32_t generate_texture(const std::filesystem::path &path, bool flip_uvs);

        /**
        * @brief Get texture format for a `number_of_channels`.
        * @param number_of_channels that the texture has.
        * @returns GL_RED, GL_RGB, GL_RGBA for the number_of_channels=[1,3,4] respectively.
        */
        static int32_t texture_format(int32_t number_of_channels);

        /**
        * @brief Initializes the cube Vertex Array Object used for skybox drawing. Caches the vao result.
        * @returns VAO of the cube used for skybox drawing.
        */
        static uint32_t init_skybox_cube();

        /**
        * @brief Check if the shader with the `shader_id` compiled successfully.
        * @returns true if the shader compilation succeeded, false otherwise.
        */
        static bool shader_compiled_successfully(uint32_t shader_id);

        /**
        *
        */
        static uint32_t compile_shader(const std::string &shader_source,
                                       resources::ShaderType shader_type);

        static uint32_t load_skybox_textures(const std::filesystem::path &path, bool flip_uvs = false);

        static void enable_depth_testing();

        static void clear_buffers();

        static std::string get_compilation_error_message(uint32_t shader_id);
    };
}
#endif //OPENGL_HPP
