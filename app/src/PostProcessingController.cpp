#include <PostProcessingController.hpp>
#include <engine/core/Controller.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <../glad/include/glad/glad.h>
#include <spdlog/spdlog.h>

#include <ProgramState.hpp>

namespace app {
    void PostProcessingController::initialize() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto window = platform->window();
        unsigned int wHeight = window->height();
        unsigned int wWidth = window->width();
        prepare_bloom_effect(wHeight, wWidth);
        prepare_filter_effect(wHeight, wWidth);
        prepare_quad();
    }

    void PostProcessingController::draw() {
        draw_bloom();
        draw_filters();
        draw_health_bar();
    }

    void PostProcessingController::prepare_bloom_effect(unsigned int wHeight, unsigned int wWidth) {
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

            attachments[0] = GL_COLOR_ATTACHMENT0;
            attachments[1] = GL_COLOR_ATTACHMENT1;
            CHECKED_GL_CALL(glDrawBuffers, 2, attachments);

            CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

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

            prepare_bloom_shaders();
        }
    }
    void PostProcessingController::prepare_filter_effect(unsigned int wHeight, unsigned int wWidth) {
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

            CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
        }
    }

    void PostProcessingController::begin_draw() {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, hdrFBO);
        engine::graphics::OpenGL::clear_buffers();
    }

    void PostProcessingController::end_draw() {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

    void PostProcessingController::prepare_bloom_shaders() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Shader* bloom = resources->shader("bloom");
        engine::resources::Shader* blur = resources->shader("blur");
        engine::resources::Shader* bloom_final = resources->shader("bloom_final");

        bloom->use();
        bloom->set_int("diffuseTexture", 0);
        blur->use();
        blur->set_int("image", 0);
        bloom_final->use();
        bloom_final->set_int("scene", 0);
        bloom_final->set_int("bloomBlur", 1);
    }

    void PostProcessingController::draw_bloom() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Shader* bloom_final = resources->shader("bloom_final");
        engine::resources::Shader* blur = resources->shader("blur");

        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

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
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

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

        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    }

    void PostProcessingController::draw_filters() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Shader* negative = resources->shader("negative");
        engine::resources::Shader* outline = resources->shader("outline");
        engine::resources::Shader* deepfried = resources->shader("deepfried");
        engine::resources::Shader* grayscale = resources->shader("grayscale");
        engine::resources::Shader* blackWhite = resources->shader("blackWhite");
        engine::resources::Shader* noFilter = resources->shader("noFilter");

        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
        CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, screenTexture);

        switch (Settings::getInstance().filter) {
        case Filter::NEGATIVE :
            negative->use();
            break;
        case Filter::GRAYSCALE :
            grayscale->use();
            break;
        case Filter::DEEPFRIED :
            deepfried->use();
            break;
        case Filter::OUTLINE :
            outline->use();
            break;
        case Filter::BLACKWHITE :
            blackWhite->use();
            break;
        case Filter::NONE :
            noFilter->use();
            break;
        }

        render_quad();
    }

    void PostProcessingController::draw_health_bar() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Shader* heartShader = resources->shader("heart");
        engine::resources::Texture* heartTex = resources->texture("heart");

        CHECKED_GL_CALL(glDisable, GL_DEPTH_TEST);
        CHECKED_GL_CALL(glEnable, GL_BLEND);
        CHECKED_GL_CALL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        heartShader->use();

        CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, heartTex->id());

        auto window = engine::core::Controller::get<engine::platform::PlatformController>()->window();
        float screenWidth = window->width();
        float screenHeight = window->height();

        const float heartSize = 50.0f;
        const float padding = 10.0f;
        const float startX = screenWidth - heartSize - padding;
        const float startY = padding + 35.0f;

        int currentHealth = Settings::getInstance().health;
        for (int i = 0; i < currentHealth; ++i) {
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

    void PostProcessingController::prepare_quad() {
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

    void PostProcessingController::render_quad() {
        CHECKED_GL_CALL(glBindVertexArray, quadVAO);
        CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
        CHECKED_GL_CALL(glBindVertexArray, 0);
    }

    void PostProcessingController::terminate() {
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
}