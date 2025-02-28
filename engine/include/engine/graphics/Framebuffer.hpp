/**
 * @file Framebuffer.hpp
 * @brief Defines some basic functions for working with framebuffers.
 */
#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <filesystem>

namespace engine::graphics {
    class Framebuffer {
    private:
        static unsigned int VAO;
    public:
        /**
        * @brief generates a framebuffer
        * @return framebuffer object
        */
        static unsigned int generate_framebuffer();

        /**
         * @brief generates two framebuffers
         * @return returns a pair of 2 framebuffers
         */
        static std::pair<unsigned int, unsigned int> generate_two_framebuffers();

        /**
         * @brief binds a given framebuffer
         * @param FBO framebuffer object that will bi bound
         */
        static void bind_framebuffer(unsigned int FBO);

        /**
         * @brief generates a texture that can be used in a framebuffer
         * @return texture id
         */
        static unsigned int generate_framebuffer_texture();

        /**
         * @brief generates 2 textures that can be used in a framebuffer
         * @return pair of texture ids
         */
        static std::pair<unsigned int, unsigned int> generate_two_framebuffer_textures();

        /**
         * @brief prepares a texture that can be used in a framebuffer
         * @param window_width width of the window
         * @param window_height height of the window
         * @param texture texture id of the texture that needs to be set up
         * @param attachment_pos at what attachment position should the texture be set
         */
        static void set_up_framebuffer_texture(unsigned int window_width, unsigned int window_height, unsigned int texture, int attachment_pos);

        /**
         * @brief binds a given texture
         * @param texture texture id of the texture that needs to be bound
         */
        static void bind_texture(unsigned int texture);

        /**
         * @brief generates a depth buffer
         * @param window_width width of the window
         * @param window_height height of the window
         */
        static void generate_depth_buffer(unsigned int window_width, unsigned int window_height);

        /**
         * @brief sets up the attachments for the framebuffer
         */
        static void set_up_attachments();

        /**
         * @brief activates a texture for a given position
         * @param texture the texture id of the texture that should be activated
         * @param pos to which position should the texture be activated
         */
        static void activate_texture(unsigned int texture, int pos);

        /**
         * @brief sets up the VAO and VBO for rendering a quad
         */
        static void set_up_quad();

        /**
         * @renders a standard quad over screen
         */
        static void render_quad();

        /**
         * @brief deletes a given framebuffer
         * @param FBO framebuffer object that needs to be deleted
         */
        static void delete_framebuffer(unsigned int FBO);

        /**
         * @brief deleted a given vertex array
         */
        static void delete_quad();
    };
}

#endif //FRAMEBUFFER_HPP
