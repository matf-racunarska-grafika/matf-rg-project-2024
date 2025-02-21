#include <glad/glad.h>
#include <engine/graphics/BloomHandler.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/util/Configuration.hpp>
#include <engine/core/Engine.hpp>
#include <spdlog/spdlog.h>
#include <iostream>

unsigned int BloomHandler::hdrFBO = 0;
unsigned int BloomHandler::colorBuffers[2] = {0, 0};
unsigned int BloomHandler::attachments[2] = {0, 0};
unsigned int BloomHandler::pingpongFBO[2] = {0, 0};
unsigned int BloomHandler::pingpongBuffer[2] = {0, 0};

engine::resources::Shader* BloomHandler::bloom = nullptr;
engine::resources::Shader* BloomHandler::blur = nullptr;
engine::resources::Shader* BloomHandler::bloom_final = nullptr;

void BloomHandler::initialise() {
    prepare_hdr_framebuffer();
    prepare_blur_framebuffers();
}

void BloomHandler::draw() {
    compose();
}

void BloomHandler::prepare_hdr_framebuffer() {
    CHECKED_GL_CALL(glGenFramebuffers, 1, &hdrFBO);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, hdrFBO);

    CHECKED_GL_CALL(glGenTextures, 2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, colorBuffers[i]);
        CHECKED_GL_CALL(glTexImage2D,
            GL_TEXTURE_2D, 0, GL_RGBA16F, 1200, 800, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        CHECKED_GL_CALL(glFramebufferTexture2D,
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1200, 800);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    prepare_attachments();
}
void BloomHandler::prepare_attachments() {
    attachments[0] = GL_COLOR_ATTACHMENT0;
    attachments[1] = GL_COLOR_ATTACHMENT1;
    CHECKED_GL_CALL(glDrawBuffers, 2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomHandler::bind_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
}

void BloomHandler::unbind_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomHandler::prepare_blur_framebuffers() {
    CHECKED_GL_CALL(glGenFramebuffers, 2, pingpongFBO);
    CHECKED_GL_CALL(glGenTextures, 2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, pingpongFBO[i]);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, pingpongBuffer[i]);
        CHECKED_GL_CALL(glTexImage2D,
            GL_TEXTURE_2D, 0, GL_RGBA16F, 1200, 800, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        CHECKED_GL_CALL(glFramebufferTexture2D,
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
    }
    prepare_shaders();
}

void BloomHandler::prepare_shaders() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    bloom = resources->shader("bloom");
    blur = resources->shader("blur");
    bloom_final = resources->shader("bloom_final");

    bloom->use();
    bloom->set_int("diffuseTexture", 0);
    blur->use();
    blur->set_int("image", 0);
    bloom_final->use();
    bloom_final->set_int("scene", 0);
    bloom_final->set_int("bloomBlur", 1);
}

void BloomHandler::render_quad() {
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void BloomHandler::compose() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;
    blur->use();
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        blur->set_int("horizontal", horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongBuffer[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
        render_quad();
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bloom_final->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
    bloom_final->set_int("bloom", true);
    bloom_final->set_float("exposure", 0.1f);
    render_quad();
}