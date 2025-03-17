#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <glad/glad.h>

namespace engine::graphics {

    unsigned int Framebuffer::VAO;

    unsigned int Framebuffer::generate_framebuffer() {
        unsigned int FBO;
        CHECKED_GL_CALL(glGenFramebuffers, 1, &FBO);
        return FBO;
    }

    std::pair<unsigned int, unsigned int> Framebuffer::generate_two_framebuffers() {
        unsigned int pingpongFBO[2];
        CHECKED_GL_CALL(glGenFramebuffers, 2, pingpongFBO);
        return {pingpongFBO[0], pingpongFBO[1]};
    }

    void Framebuffer::bind_framebuffer(unsigned int FBO) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, FBO);
    }

    std::pair<unsigned int, unsigned int> Framebuffer::generate_two_framebuffer_textures() {
        unsigned int color_buffers[2];
        CHECKED_GL_CALL(glGenTextures, 2, color_buffers);
        return {color_buffers[0], color_buffers[1]};
    }

    unsigned int Framebuffer::generate_framebuffer_texture() {
        unsigned int texture;
        CHECKED_GL_CALL(glGenTextures, 1, &texture);
        return texture;
    }

    void Framebuffer::set_up_framebuffer_texture(unsigned int window_width, unsigned int window_height,
                                                 unsigned int texture, int attachment_pos) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, texture);
        CHECKED_GL_CALL(glTexImage2D,
                        GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        CHECKED_GL_CALL(glFramebufferTexture2D,
                        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment_pos, GL_TEXTURE_2D, texture, 0);
    }

    void Framebuffer::bind_texture(unsigned int texture) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, texture);
    }

    void Framebuffer::generate_depth_buffer(unsigned int window_width, unsigned int window_height) {
        unsigned int rboDepth;
        CHECKED_GL_CALL(glGenRenderbuffers, 1, &rboDepth);
        CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, rboDepth);
        CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
        CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    }

    void Framebuffer::set_up_attachments() {
        unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        CHECKED_GL_CALL(glDrawBuffers, 2, attachments);
    }

    void Framebuffer::activate_texture(unsigned int texture, int pos) {
        CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0 + pos);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, texture);
    }

    void Framebuffer::set_up_quad() {
        unsigned int quadVBO;
        float quad_vertices[] = {
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        CHECKED_GL_CALL(glGenVertexArrays, 1, &VAO);
        CHECKED_GL_CALL(glGenBuffers, 1, &quadVBO);
        CHECKED_GL_CALL(glBindVertexArray, VAO);
        CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, quadVBO);
        CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 0);
        CHECKED_GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 1);
        CHECKED_GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    void Framebuffer::render_quad() {
        CHECKED_GL_CALL(glBindVertexArray, VAO);
        CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
        CHECKED_GL_CALL(glBindVertexArray, 0);
    }

    void Framebuffer::delete_framebuffer(unsigned int FBO) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &FBO);
    }

    void Framebuffer::delete_quad() {
        CHECKED_GL_CALL(glDeleteVertexArrays, 1, &VAO);
    }
}
