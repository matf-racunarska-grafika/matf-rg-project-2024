#include <engine/graphics/PostProcessingController.hpp>
#include <engine/core/Controller.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <spdlog/spdlog.h>

namespace engine::graphics {
    void PostProcessingController::initialize() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto window = platform->window();
        unsigned int wHeight = window->height();
        unsigned int wWidth = window->width();

        prepare_bloom_effect(wHeight, wWidth);
        prepare_filter_effect(wHeight, wWidth);
        engine::graphics::Framebuffer::set_up_quad();
    }

    void PostProcessingController::draw() {
        draw_bloom();
        draw_filters();
    }

    void PostProcessingController::prepare_bloom_effect(unsigned int wHeight, unsigned int wWidth) {
        if (m_hdrFBO == 0) {
            m_hdrFBO = engine::graphics::Framebuffer::generate_framebuffer();
            engine::graphics::Framebuffer::bind_framebuffer(m_hdrFBO);

            std::tie(m_colorBuffers[0], m_colorBuffers[1]) = engine::graphics::Framebuffer::generate_two_framebuffer_textures();
            for (unsigned int i = 0; i < 2; i++) {
                engine::graphics::Framebuffer::set_up_framebuffer_texture(wWidth, wHeight, m_colorBuffers[i], i);
            }
            engine::graphics::Framebuffer::generate_depth_buffer(wWidth, wHeight);
            engine::graphics::Framebuffer::set_up_attachments();
            engine::graphics::Framebuffer::bind_framebuffer(0);

            std::tie(m_pingpongFBO[0], m_pingpongFBO[1]) = engine::graphics::Framebuffer::generate_two_framebuffers();
            std::tie(m_pingpongBuffer[0], m_pingpongBuffer[1]) = engine::graphics::Framebuffer::generate_two_framebuffer_textures();
            for (unsigned int i = 0; i < 2; i++) {
                engine::graphics::Framebuffer::bind_framebuffer(m_pingpongFBO[i]);
                engine::graphics::Framebuffer::set_up_framebuffer_texture(wWidth, wHeight, m_pingpongBuffer[i], 0);
            }
            prepare_bloom_shaders();
        }
    }
    void PostProcessingController::prepare_filter_effect(unsigned int wHeight, unsigned int wWidth) {
        if (m_screenFBO == 0) {
            m_screenFBO = engine::graphics::Framebuffer::generate_framebuffer();
            engine::graphics::Framebuffer::bind_framebuffer(m_screenFBO);

            m_screenTexture = engine::graphics::Framebuffer::generate_framebuffer_texture();
            engine::graphics::Framebuffer::set_up_framebuffer_texture(wWidth, wHeight, m_screenTexture, 0);

            engine::graphics::Framebuffer::bind_framebuffer(0);
        }
    }

    void PostProcessingController::begin_draw() {
        engine::graphics::Framebuffer::bind_framebuffer(m_hdrFBO);
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
            engine::graphics::Framebuffer::bind_framebuffer(m_pingpongFBO[horizontal]);
            blur->set_int("horizontal", horizontal);
            engine::graphics::Framebuffer::bind_texture(first_iteration ? m_colorBuffers[1] : m_pingpongBuffer[!horizontal]);
            engine::graphics::Framebuffer::render_quad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        engine::graphics::Framebuffer::bind_framebuffer(0);

        engine::graphics::Framebuffer::bind_framebuffer(m_screenFBO);
        engine::graphics::OpenGL::clear_buffers(false);
        bloom_final->use();
        engine::graphics::Framebuffer::activate_texture(m_colorBuffers[0], 0);
        engine::graphics::Framebuffer::activate_texture(m_pingpongBuffer[!horizontal], 1);
        bloom_final->set_int("bloom", true);
        bloom_final->set_float("exposure", 0.2f);
        engine::graphics::Framebuffer::render_quad();

        engine::graphics::Framebuffer::bind_framebuffer(0);
    }

    void PostProcessingController::draw_filters() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Shader* post_processing_shader = resources->shader("postProcessing");
        post_processing_shader->use();
        post_processing_shader->set_int("effectType", static_cast<int>(m_active_filter));

        engine::graphics::Framebuffer::bind_framebuffer(0);
        engine::graphics::OpenGL::clear_buffers(false);
        engine::graphics::Framebuffer::activate_texture(m_screenTexture, 0);

        engine::graphics::Framebuffer::render_quad();
    }

    void PostProcessingController::terminate() {
        if (m_hdrFBO != 0) {
            engine::graphics::Framebuffer::delete_framebuffer(m_hdrFBO);
            m_hdrFBO = 0;
        }
        if (m_screenFBO != 0) {
            engine::graphics::Framebuffer::delete_framebuffer(m_screenFBO);
            m_screenFBO = 0;
        }
        for (unsigned int i = 0; i < 2; i++) {
            if (m_pingpongFBO[i] != 0) {
                engine::graphics::Framebuffer::delete_framebuffer(m_pingpongFBO[i]);
                m_pingpongFBO[i] = 0;
            }
        }
        if (m_colorBuffers[0] != 0 || m_colorBuffers[1] != 0) {
            engine::graphics::OpenGL::delete_texture(m_colorBuffers[0]);
            m_colorBuffers[0] = m_colorBuffers[1] = 0;
        }
        if (m_pingpongBuffer[0] != 0 || m_pingpongBuffer[1] != 0) {
            engine::graphics::OpenGL::delete_texture(m_pingpongBuffer[0]);
            engine::graphics::OpenGL::delete_texture(m_pingpongBuffer[1]);
            m_pingpongBuffer[0] = m_pingpongBuffer[1] = 0;
        }
        if (m_screenTexture != 0) {
            engine::graphics::OpenGL::delete_texture(m_screenTexture);
            m_screenTexture = 0;
        }
        engine::graphics::Framebuffer::delete_quad();
    }

    void PostProcessingController::set_active_filter(Filter filter) {
        m_active_filter = filter;
    }

    Filter PostProcessingController::get_active_filter() {
        return m_active_filter;
    }
}