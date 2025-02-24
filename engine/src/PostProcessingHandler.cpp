#include <glad/glad.h>
#include <engine/core/Engine.hpp>
#include <engine/core/Controller.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/PostProcessingHandler.hpp>
#include <engine/util/Configuration.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

#include "../../app/include/ProgramState.hpp"

unsigned int PostProcessingHandler::hdrFBO = 0;
unsigned int PostProcessingHandler::colorBuffers[2] = {0, 0};
unsigned int PostProcessingHandler::attachments[2] = {0, 0};
unsigned int PostProcessingHandler::pingpongFBO[2] = {0, 0};
unsigned int PostProcessingHandler::pingpongBuffer[2] = {0, 0};
unsigned int PostProcessingHandler::screenFBO = 0;
unsigned int PostProcessingHandler::screenTexture = 0;
unsigned int PostProcessingHandler::quadVAO = 0;
unsigned int PostProcessingHandler::quadVBO = 0;

engine::resources::Shader* PostProcessingHandler::bloom = nullptr;
engine::resources::Shader* PostProcessingHandler::blur = nullptr;
engine::resources::Shader* PostProcessingHandler::bloom_final = nullptr;
engine::resources::Shader* PostProcessingHandler::negative = nullptr;
engine::resources::Shader* PostProcessingHandler::grayscale = nullptr;
engine::resources::Shader* PostProcessingHandler::deepfried = nullptr;
engine::resources::Shader* PostProcessingHandler::none = nullptr;
engine::resources::Shader* PostProcessingHandler::outline = nullptr;
engine::resources::Shader* PostProcessingHandler::blackWhite = nullptr;
engine::resources::Texture* PostProcessingHandler::heartTex = nullptr;
engine::resources::Shader* PostProcessingHandler::heartShader = nullptr;

void PostProcessingHandler::initialise() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto window = platform->window();
    unsigned int wHeight = window->height();
    unsigned int wWidth = window->width();
    prepare_post_processing_framebuffer(wHeight, wWidth);
    prepare_hdr_framebuffer(wHeight, wWidth);
    prepare_blur_framebuffers(wHeight, wWidth);
    prepare_quad();
}

void PostProcessingHandler::draw() {
    compose();
}

void PostProcessingHandler::prepare_hdr_framebuffer(unsigned int wHeight, unsigned int wWidth) {
    if (hdrFBO == 0) {
        CHECKED_GL_CALL(glGenFramebuffers, 1, &hdrFBO);
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, hdrFBO);

        CHECKED_GL_CALL(glGenTextures, 2, colorBuffers);
        for (unsigned int i = 0; i < 2; i++) {
            CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, colorBuffers[i]);
            CHECKED_GL_CALL(glTexImage2D,
                GL_TEXTURE_2D, 0, GL_RGBA16F, wWidth, wHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
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
        CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, wWidth, wHeight);
        CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        prepare_attachments();
    }
}
void PostProcessingHandler::prepare_post_processing_framebuffer(unsigned int wHeight, unsigned int wWidth) {
    if (screenFBO == 0) {
        CHECKED_GL_CALL(glGenFramebuffers, 1, &screenFBO);
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, screenFBO);

        CHECKED_GL_CALL(glGenTextures, 1, &screenTexture);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, screenTexture);
        CHECKED_GL_CALL(glTexImage2D,
            GL_TEXTURE_2D, 0, GL_RGBA16F, wWidth, wHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        CHECKED_GL_CALL(glFramebufferTexture2D,
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

        unbind_framebuffer();
    }
}
void PostProcessingHandler::prepare_attachments() {
    attachments[0] = GL_COLOR_ATTACHMENT0;
    attachments[1] = GL_COLOR_ATTACHMENT1;
    CHECKED_GL_CALL(glDrawBuffers, 2, attachments);

    unbind_framebuffer();
}

void PostProcessingHandler::bind_hdr_framebuffer() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, hdrFBO);
}

void PostProcessingHandler::unbind_framebuffer() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void PostProcessingHandler::prepare_blur_framebuffers(unsigned int wHeight, unsigned int wWidth) {
    CHECKED_GL_CALL(glGenFramebuffers, 2, pingpongFBO);
    CHECKED_GL_CALL(glGenTextures, 2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, pingpongFBO[i]);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, pingpongBuffer[i]);
        CHECKED_GL_CALL(glTexImage2D,
            GL_TEXTURE_2D, 0, GL_RGBA16F, wWidth, wHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        CHECKED_GL_CALL(glFramebufferTexture2D,
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
    }

    prepare_shaders();
}

void PostProcessingHandler::prepare_shaders() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    bloom = resources->shader("bloom");
    blur = resources->shader("blur");
    bloom_final = resources->shader("bloom_final");
    negative = resources->shader("negative");
    grayscale = resources->shader("grayscale");
    deepfried = resources->shader("deepfried");
    none = resources->shader("none");
    outline = resources->shader("outline");
    blackWhite = resources->shader("blackWhite");

    heartTex = resources->texture("heart", "textures/heart.png", engine::resources::TextureType::Regular, true);
    heartShader = resources->shader("heart");

    bloom->use();
    bloom->set_int("diffuseTexture", 0);
    blur->use();
    blur->set_int("image", 0);
    bloom_final->use();
    bloom_final->set_int("scene", 0);
    bloom_final->set_int("bloomBlur", 1);
}

void PostProcessingHandler::compose() {
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

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, screenFBO);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bloom_final->use();
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, colorBuffers[0]);
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE1);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
    bloom_final->set_int("bloom", true);
    bloom_final->set_float("exposure", 0.2f);
    render_quad();
    unbind_framebuffer();

    apply_filters();
    render_health();
}

void PostProcessingHandler::apply_filters() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, screenTexture);

    switch (Settings::getInstance().filter) {
    case Filter::NEGATIVE :
        negative->use();
        negative->set_int("screenTexture", 0);
        break;
    case Filter::GRAYSCALE :
        grayscale->use();
        grayscale->set_int("screenTexture", 0);
        break;
    case Filter::DEEPFRIED :
        deepfried->use();
        deepfried->set_int("screenTexture", 0);
        break;
    case Filter::OUTLINE :
        outline->use();
        outline->set_int("screenTexture", 0);
        break;
    case Filter::BLACKWHITE :
        blackWhite->use();
        blackWhite->set_int("screenTexture", 0);
        break;
    case Filter::NONE :
        none->use();
        none->set_int("screenTexture", 0);
        break;
    }

    render_quad();
}

void PostProcessingHandler::render_health() {
    CHECKED_GL_CALL(glDisable, GL_DEPTH_TEST);
    CHECKED_GL_CALL(glEnable, GL_BLEND);
    CHECKED_GL_CALL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    heartShader->use();
    heartShader->set_int("heartTexture", 0);

    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, heartTex->id());

    auto window = engine::core::Controller::get<engine::platform::PlatformController>()->window();
    float screenWidth = window->width();
    float screenHeight = window->height();

    const float heartSize = 50.0f;
    const float padding = 10.0f;
    const float startX = screenWidth - heartSize - padding;
    const float startY = padding + 35.0f;

    int health = Settings::getInstance().health;
    for (int i = 0; i < health; ++i) {
        float x = startX - i * (heartSize + padding);
        float y = startY;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(
            (x / screenWidth) * 2.0f - 1.0f,
            1.0f - (y / screenHeight) * 2.0f,
            0.0f
        ));
        model = glm::scale(model, glm::vec3(
            heartSize / screenWidth,
            heartSize / screenHeight,
            1.0f
        ));

        heartShader->set_mat4("model", model);
        render_quad();
    }
    CHECKED_GL_CALL(glDisable, GL_BLEND);
    CHECKED_GL_CALL(glEnable, GL_DEPTH_TEST);
}

void PostProcessingHandler::prepare_quad() {
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
}

void PostProcessingHandler::render_quad() {
    CHECKED_GL_CALL(glBindVertexArray, quadVAO);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}

void PostProcessingHandler::cleanup() {
    if (hdrFBO != 0) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &hdrFBO);
        hdrFBO = 0;
    }
    if (screenFBO != 0) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &screenFBO);
        screenFBO = 0;
    }
    for (unsigned int i = 0; i < 2; i++) {
        if (pingpongFBO[i] != 0) {
            CHECKED_GL_CALL(glDeleteFramebuffers, 1, &pingpongFBO[i]);
            pingpongFBO[i] = 0;
        }
    }
    if (colorBuffers[0] != 0 || colorBuffers[1] != 0) {
        CHECKED_GL_CALL(glDeleteTextures, 2, colorBuffers);
        colorBuffers[0] = colorBuffers[1] = 0;
    }
    if (pingpongBuffer[0] != 0 || pingpongBuffer[1] != 0) {
        CHECKED_GL_CALL(glDeleteTextures, 2, pingpongBuffer);
        pingpongBuffer[0] = pingpongBuffer[1] = 0;
    }
    if (screenTexture != 0) {
        CHECKED_GL_CALL(glDeleteTextures, 1, &screenTexture);
        screenTexture = 0;
    }

    if (quadVAO != 0) {
        CHECKED_GL_CALL(glDeleteVertexArrays, 1, &quadVAO);
        quadVAO = 0;
    }
    if (quadVBO != 0) {
        CHECKED_GL_CALL(glDeleteBuffers, 1, &quadVBO);
        quadVBO = 0;
    }
}

void PostProcessingHandler::cleanup() {
    if (hdrFBO != 0) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &hdrFBO);
        hdrFBO = 0;
    }
    if (screenFBO != 0) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &screenFBO);
        screenFBO = 0;
    }
    for (unsigned int i = 0; i < 2; i++) {
        if (pingpongFBO[i] != 0) {
            CHECKED_GL_CALL(glDeleteFramebuffers, 1, &pingpongFBO[i]);
            pingpongFBO[i] = 0;
        }
    }
    if (colorBuffers[0] != 0 || colorBuffers[1] != 0) {
        CHECKED_GL_CALL(glDeleteTextures, 2, colorBuffers);
        colorBuffers[0] = colorBuffers[1] = 0;
    }
    if (pingpongBuffer[0] != 0 || pingpongBuffer[1] != 0) {
        CHECKED_GL_CALL(glDeleteTextures, 2, pingpongBuffer);
        pingpongBuffer[0] = pingpongBuffer[1] = 0;
    }
    if (screenTexture != 0) {
        CHECKED_GL_CALL(glDeleteTextures, 1, &screenTexture);
        screenTexture = 0;
    }

    if (quadVAO != 0) {
        CHECKED_GL_CALL(glDeleteVertexArrays, 1, &quadVAO);
        quadVAO = 0;
    }
    if (quadVBO != 0) {
        CHECKED_GL_CALL(glDeleteBuffers, 1, &quadVBO);
        quadVBO = 0;
    }
}