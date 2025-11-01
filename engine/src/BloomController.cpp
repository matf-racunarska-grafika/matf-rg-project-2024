#include <glad/glad.h>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/BloomController.h>

namespace engine::graphics {

void BloomController::render_quad() {
    if (m_quad_vao == 0) {
        float vertices[] = {
            // pos        // tex
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        CHECKED_GL_CALL(glGenVertexArrays, 1, &m_quad_vao);
        CHECKED_GL_CALL(glGenBuffers, 1, &m_quad_vbo);
        CHECKED_GL_CALL(glBindVertexArray, m_quad_vao);
        CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_quad_vbo);
        CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        CHECKED_GL_CALL(glEnableVertexAttribArray, 0);
        CHECKED_GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 1);
        CHECKED_GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    CHECKED_GL_CALL(glBindVertexArray, m_quad_vao);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}

void BloomController::bloom_setup() {
    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_hdr_fbo);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_hdr_fbo);

    CHECKED_GL_CALL(glGenTextures, 2, m_color_buffers);

    m_scr_width  = get<platform::PlatformController>()->window()->width();
    m_scr_height = get<platform::PlatformController>()->window()->height();

    for (unsigned int i = 0; i < 2; ++i) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_color_buffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, m_scr_width, m_scr_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_color_buffers[i], 0);
    }

    unsigned int rbo_depth;
    CHECKED_GL_CALL(glGenRenderbuffers, 1, &rbo_depth);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, rbo_depth);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_scr_width, m_scr_height);
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    CHECKED_GL_CALL(glDrawBuffers, 2, attachments);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

    CHECKED_GL_CALL(glGenFramebuffers, 2, m_pingpong_fbo);
    CHECKED_GL_CALL(glGenTextures, 2, m_pingpong_colorbuffers);

    for (unsigned int i = 0; i < 2; ++i) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_pingpong_fbo[i]);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_pingpong_colorbuffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, m_scr_width, m_scr_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpong_colorbuffers[i], 0);
    }
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

    const auto res = get<resources::ResourcesController>();
    auto finalShader = res->shader("bloom_final");
    finalShader->use();
    finalShader->set_int("scene", 0);
    finalShader->set_int("bloomBlur", 1);
}

void BloomController::render_bloom() {
    const auto res = get<resources::ResourcesController>();
    auto blurShader  = res->shader("blur");
    auto finalShader = res->shader("bloom_final");

    bool horizontal = true, first = true;
    blurShader->use();

    for (unsigned int i = 0; i < bloom_passes; ++i) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_pingpong_fbo[horizontal]);
        blurShader->set_int("horizontal", horizontal);
        CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, first ? m_color_buffers[1] : m_pingpong_colorbuffers[!horizontal]);
        render_quad();
        horizontal = !horizontal;
        if (first) first = false;
    }

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

    finalShader->use();
    finalShader->set_bool("bloom", bloom);
    finalShader->set_float("exposure", exposure);
    finalShader->set_float("bloomStrength", bloom_strength);

    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_color_buffers[0]);
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE1);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_pingpong_colorbuffers[!horizontal]);

    render_quad();
}

void BloomController::prepare_hdr() {
    m_scr_width  = get<platform::PlatformController>()->window()->width();
    m_scr_height = get<platform::PlatformController>()->window()->height();
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_hdr_fbo);
    CHECKED_GL_CALL(glViewport, 0, 0, m_scr_width, m_scr_height);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomController::finalize_bloom() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    render_bloom();
}

void BloomController::terminate() {
    CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_hdr_fbo);
    CHECKED_GL_CALL(glDeleteFramebuffers, 2, m_pingpong_fbo);
    CHECKED_GL_CALL(glDeleteTextures, 2, m_color_buffers);
    CHECKED_GL_CALL(glDeleteTextures, 2, m_pingpong_colorbuffers);
    if (m_quad_vao) CHECKED_GL_CALL(glDeleteVertexArrays, 1, &m_quad_vao);
    if (m_quad_vbo) CHECKED_GL_CALL(glDeleteBuffers, 1, &m_quad_vbo);
    m_quad_vao = m_quad_vbo = 0;
}

void BloomController::toggle_bloom() {
    bloom = !bloom;
}

void BloomController::set_bloom(bool enabled) {
    bloom = enabled;
}

bool BloomController::is_bloom_enabled() const {
    return bloom;
}

} // namespace engine::graphics


