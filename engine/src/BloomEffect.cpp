#include "engine/graphics/BloomEffect.hpp"
#include <engine/resources/Shader.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

using namespace engine::graphics;

BloomEffect::BloomEffect() = default;

BloomEffect::~BloomEffect() {
    glDeleteFramebuffers(1, &sceneFBO_);
    glDeleteTextures(1, &sceneColorTex_);
    glDeleteTextures(1, &brightTex_);
    glDeleteRenderbuffers(1, &depthRBO_);
    for (int i = 0; i < 2; ++i) {
        glDeleteFramebuffers(1, &pingpongFBO_[i]);
        glDeleteTextures(1, &pingpongTex_[i]);
    }
    if (quadVAO_) glDeleteVertexArrays(1, &quadVAO_);
    if (quadVBO_) glDeleteBuffers(1, &quadVBO_);
}

void BloomEffect::initialize(int w, int h, resources::Shader* blurShader,resources::Shader* finalShader) {
    width_ = w;
    height_ = h;
    blurShader_ = blurShader;
    finalShader_ = finalShader;


    glGenFramebuffers(1, &sceneFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO_);

    // glavna boja
    glGenTextures(1, &sceneColorTex_);
    glBindTexture(GL_TEXTURE_2D, sceneColorTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColorTex_, 0);

    // samo svetli pikseli
    glGenTextures(1, &brightTex_);
    glBindTexture(GL_TEXTURE_2D, brightTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightTex_, 0);

    // rbo
    glGenRenderbuffers(1, &depthRBO_);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO_);


    GLenum bufs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, bufs);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        spdlog::error("Framebuffer not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //ping-pong
    for (int i = 0; i < 2; ++i) {
        glGenFramebuffers(1, &pingpongFBO_[i]);
        glGenTextures(1, &pingpongTex_[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO_[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongTex_[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongTex_[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            spdlog::error("Pingpong FBO %d not complete!", i);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomEffect::resize(int w, int h) {
    // ponovo kreiraj sve pri resize
    initialize(w, h, blurShader_, finalShader_);
}

void BloomEffect::begin() {
    // prebaci render u sceneFBO
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO_);
    GLenum bufs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, bufs);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomEffect::end(const glm::vec2& vp) {
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    bool horizontal = true;
    bool firstPass = true;
    const int blurPasses = 10;

    // blur iteracije gaus
    blurShader_->use();
    for (int i = 0; i < blurPasses; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO_[horizontal]);
        blurShader_->set_int("horizontal", horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,
            firstPass ? brightTex_ : pingpongTex_[!horizontal]);
        drawQuad();
        horizontal = !horizontal;
        if (firstPass) firstPass = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, (int)vp.x, (int)vp.y);
    if (!finalShader_) {
        spdlog::error("Final shader not initialized");
        return;
    }
    finalShader_->use();
    finalShader_->set_int("scene", 0);
    finalShader_->set_int("bloomBlur", 1);
    finalShader_->set_float("bloomIntensity", 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneColorTex_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongTex_[!horizontal]);
    drawQuad();

    glEnable(GL_DEPTH_TEST);
}

void BloomEffect::drawQuad() {
    if (quadVAO_ == 0) {
        float verts[] = {
            // positions // texture Coords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO_);
        glGenBuffers(1, &quadVBO_);
        glBindVertexArray(quadVAO_);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                              4 * sizeof(float), (void*)(2 * sizeof(float)));
    }
    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
