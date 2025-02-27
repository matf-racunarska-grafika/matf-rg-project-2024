#include <PostProcessingController.hpp>
#include <engine/core/Controller.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/Framebuffer.hpp>
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
        quadVAO = engine::graphics::Framebuffer::set_up_quad();
    }

    void PostProcessingController::draw() {
        draw_bloom();
        draw_filters();
        draw_health_bar();
    }

    void PostProcessingController::prepare_bloom_effect(unsigned int wHeight, unsigned int wWidth) {
        if (hdrFBO == 0) {
            hdrFBO = engine::graphics::Framebuffer::generate_framebuffer();
            engine::graphics::Framebuffer::bind_framebuffer(hdrFBO);

            std::tie(colorBuffers[0], colorBuffers[1]) = engine::graphics::Framebuffer::generate_two_framebuffer_textures();
            for (unsigned int i = 0; i < 2; i++) {
                engine::graphics::Framebuffer::set_up_framebuffer_texture(wWidth, wHeight, colorBuffers[i], i);
            }
            engine::graphics::Framebuffer::generate_depth_buffer(wWidth, wHeight);
            engine::graphics::Framebuffer::set_up_attachments();
            engine::graphics::Framebuffer::bind_framebuffer(0);

            std::tie(pingpongFBO[0], pingpongFBO[1]) = engine::graphics::Framebuffer::generate_two_framebuffers();
            std::tie(pingpongBuffer[0], pingpongBuffer[1]) = engine::graphics::Framebuffer::generate_two_framebuffer_textures();
            for (unsigned int i = 0; i < 2; i++) {
                engine::graphics::Framebuffer::bind_framebuffer(pingpongFBO[i]);
                engine::graphics::Framebuffer::set_up_framebuffer_texture(wWidth, wHeight, pingpongBuffer[i], 0);
            }
            prepare_bloom_shaders();
        }
    }
    void PostProcessingController::prepare_filter_effect(unsigned int wHeight, unsigned int wWidth) {
        if (screenFBO == 0) {
            screenFBO = engine::graphics::Framebuffer::generate_framebuffer();
            engine::graphics::Framebuffer::bind_framebuffer(screenFBO);

            screenTexture = engine::graphics::Framebuffer::generate_framebuffer_texture();
            engine::graphics::Framebuffer::set_up_framebuffer_texture(wWidth, wHeight, screenTexture, 0);

            engine::graphics::Framebuffer::bind_framebuffer(0);
        }
    }

    void PostProcessingController::begin_draw() {
        engine::graphics::Framebuffer::bind_framebuffer(hdrFBO);
        engine::graphics::OpenGL::clear_buffers();
    }

    void PostProcessingController::end_draw() {
        engine::graphics::Framebuffer::bind_framebuffer(0);
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

        engine::graphics::Framebuffer::bind_framebuffer(0);

        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        blur->use();
        for (unsigned int i = 0; i < amount; i++)
        {
            engine::graphics::Framebuffer::bind_framebuffer(pingpongFBO[horizontal]);
            blur->set_int("horizontal", horizontal);
            engine::graphics::Framebuffer::bind_texture(first_iteration ? colorBuffers[1] : pingpongBuffer[!horizontal]);
            engine::graphics::Framebuffer::render_quad(quadVAO);
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        engine::graphics::Framebuffer::bind_framebuffer(0);

        engine::graphics::Framebuffer::bind_framebuffer(screenFBO);
        engine::graphics::OpenGL::clear_buffers(false);
        bloom_final->use();
        engine::graphics::Framebuffer::activate_texture(colorBuffers[0], 0);
        engine::graphics::Framebuffer::activate_texture(pingpongBuffer[!horizontal], 1);
        bloom_final->set_int("bloom", true);
        bloom_final->set_float("exposure", 0.2f);
        engine::graphics::Framebuffer::render_quad(quadVAO);

        engine::graphics::Framebuffer::bind_framebuffer(0);
    }

    void PostProcessingController::draw_filters() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Shader* negative = resources->shader("negative");
        engine::resources::Shader* outline = resources->shader("outline");
        engine::resources::Shader* deepfried = resources->shader("deepfried");
        engine::resources::Shader* grayscale = resources->shader("grayscale");
        engine::resources::Shader* blackWhite = resources->shader("blackWhite");
        engine::resources::Shader* noFilter = resources->shader("noFilter");

        engine::graphics::Framebuffer::bind_framebuffer(0);
        engine::graphics::OpenGL::clear_buffers(false);
        engine::graphics::Framebuffer::activate_texture(screenTexture, 0);

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
        engine::graphics::Framebuffer::render_quad(quadVAO);
    }

    void PostProcessingController::draw_health_bar() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Shader* heartShader = resources->shader("heart");
        engine::resources::Texture* heartTex = resources->texture("heart");

        engine::graphics::OpenGL::disable_depth_testing();
        engine::graphics::OpenGL::enable_blend();
        engine::graphics::OpenGL::gl_blend_func();

        heartShader->use();
        engine::graphics::Framebuffer::activate_texture(heartTex->id(), 0);

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
            engine::graphics::Framebuffer::render_quad(quadVAO);
        }
        engine::graphics::OpenGL::disable_blend();
        engine::graphics::OpenGL::enable_depth_testing();
    }

    void PostProcessingController::terminate() {
        if (hdrFBO != 0) {
            engine::graphics::Framebuffer::delete_framebuffer(hdrFBO);
            hdrFBO = 0;
        }
        if (screenFBO != 0) {
            engine::graphics::Framebuffer::delete_framebuffer(screenFBO);
            screenFBO = 0;
        }
        for (unsigned int i = 0; i < 2; i++) {
            if (pingpongFBO[i] != 0) {
                engine::graphics::Framebuffer::delete_framebuffer(pingpongFBO[i]);
                pingpongFBO[i] = 0;
            }
        }
        if (colorBuffers[0] != 0 || colorBuffers[1] != 0) {
            engine::graphics::OpenGL::delete_texture(colorBuffers[0]);
            colorBuffers[0] = colorBuffers[1] = 0;
        }
        if (pingpongBuffer[0] != 0 || pingpongBuffer[1] != 0) {
            engine::graphics::OpenGL::delete_texture(pingpongBuffer[0]);
            engine::graphics::OpenGL::delete_texture(pingpongBuffer[1]);
            pingpongBuffer[0] = pingpongBuffer[1] = 0;
        }
        if (screenTexture != 0) {
            engine::graphics::OpenGL::delete_texture(screenTexture);
            screenTexture = 0;
        }

        if (quadVAO != 0) {
            engine::graphics::Framebuffer::delete_vertex_array(quadVAO);
            quadVAO = 0;
        }
    }
}