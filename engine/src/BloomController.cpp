#include <engine/graphics/BloomController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/core/App.hpp>

#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace engine::graphics {

std::string_view BloomController::name() const { return "BloomController"; }

void BloomController::initialize() {
    auto resources = core::Controller::get<resources::ResourcesController>();

    // Load shaders through ResourcesController
    m_bloomShader = std::unique_ptr<resources::Shader>(resources->shader("bloom_extract"));
    m_blurShader = std::unique_ptr<resources::Shader>(resources->shader("bloom_blur"));
    m_finalShader = std::unique_ptr<resources::Shader>(resources->shader("bloom_final"));

    if (!m_bloomShader || !m_blurShader || !m_finalShader) {
        spdlog::error("Failed to load bloom shaders");
        return;
    }

    create_framebuffers();
    create_quad();

    m_initialized = true;
    spdlog::info("BloomController initialized");
}

void BloomController::terminate() {
    if (m_hdrFBO) {
        glDeleteFramebuffers(1, &m_hdrFBO);
        glDeleteTextures(2, m_colorBuffers);
    }

    for (int i = 0; i < 2; i++) {
        if (m_pingpongFBO[i]) {
            glDeleteFramebuffers(1, &m_pingpongFBO[i]);
            glDeleteTextures(1, &m_pingpongColorbuffers[i]);
        }
    }

    if (m_quadVAO) {
        glDeleteVertexArrays(1, &m_quadVAO);
        glDeleteBuffers(1, &m_quadVBO);
    }
}

void BloomController::create_framebuffers() {
    auto platform = core::Controller::get<platform::PlatformController>();
    auto window = platform->window();
    int width = window->width();
    int height = window->height();

    // Create HDR FBO
    glGenFramebuffers(1, &m_hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);

    // Create two color buffers (normal + bright)
    glGenTextures(2, m_colorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, m_colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorBuffers[i], 0);
    }

    // Create depth buffer
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // Tell OpenGL which color attachments we'll use
    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { spdlog::error("Framebuffer not complete!"); }

    // Ping-pong framebuffers for blurring
    glGenFramebuffers(2, m_pingpongFBO);
    glGenTextures(2, m_pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongColorbuffers[i], 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomController::create_quad() {
    float quadVertices[] = {
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
}

void BloomController::render_quad() {
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void BloomController::begin_frame() {
    if (!m_enabled || !m_initialized) return;

    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomController::apply_bloom() {
    if (!m_enabled || !m_initialized) return;

    // 1. Extract bright areas
    glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[0]);
    m_bloomShader->use();
    m_bloomShader->set_float("threshold", m_bloomThreshold);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_colorBuffers[1]);// Bright areas texture
    render_quad();

    // 2. Blur with two-pass Gaussian blur
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;// Number of blur passes

    m_blurShader->use();
    for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[horizontal]);
        m_blurShader->set_bool("horizontal", horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? m_pingpongColorbuffers[!horizontal] : m_pingpongColorbuffers[!horizontal]);
        render_quad();
        horizontal = !horizontal;
        if (first_iteration) first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Combine original scene with bloom
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_finalShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_colorBuffers[0]);// Original scene
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[!horizontal]);// Blurred bloom
    m_finalShader->set_float("bloomIntensity", m_bloomIntensity);
    render_quad();
}

}// namespace engine::graphics