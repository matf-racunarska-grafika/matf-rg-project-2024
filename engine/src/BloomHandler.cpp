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
    CHECKED_GL_CALL(glGenRenderbuffers, 1, &rboDepth);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, rboDepth);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1200, 800);
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    prepare_attachments();
}
void BloomHandler::prepare_attachments() {
    attachments[0] = GL_COLOR_ATTACHMENT0;
    attachments[1] = GL_COLOR_ATTACHMENT1;
    CHECKED_GL_CALL(glDrawBuffers, 2, attachments);

    unbind_framebuffer();
}

void BloomHandler::bind_framebuffer() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, hdrFBO);
}

void BloomHandler::unbind_framebuffer() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
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
        CHECKED_GL_CALL(glGenVertexArrays, 1, &quadVAO);
        CHECKED_GL_CALL(glGenBuffers, 1, &quadVBO);
        CHECKED_GL_CALL(glBindVertexArray, quadVAO);
        CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, quadVBO);
        CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 0);
        CHECKED_GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float) , (void*)0);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 1);
        CHECKED_GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    CHECKED_GL_CALL(glBindVertexArray, quadVAO);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}

void BloomHandler::compose() {
    unbind_framebuffer();

    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;
    blur->use();
    for (unsigned int i = 0; i < amount; i++)
    {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        blur->set_int("horizontal", horizontal);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongBuffer[!horizontal]);
        render_quad();
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    unbind_framebuffer();

    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bloom_final->use();
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, colorBuffers[0]);
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE1);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
    bloom_final->set_int("bloom", true);
    bloom_final->set_float("exposure", 0.18f);
    render_quad();
}
