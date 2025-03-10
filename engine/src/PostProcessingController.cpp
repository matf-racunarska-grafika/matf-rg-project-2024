#include <engine/core/Controller.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/PostProcessingController.hpp>
#include <spdlog/spdlog.h>

namespace engine::graphics {
    void PostProcessingController::initialize() {
        auto platform        = get<platform::PlatformController>();
        auto window          = platform->window();
        unsigned int wHeight = window->height();
        unsigned int wWidth  = window->width();

        prepare_bloom_effect(wHeight, wWidth);
        prepare_filter_effect(wHeight, wWidth);
        Framebuffer::set_up_quad();
    }

    void PostProcessingController::draw() {
        draw_bloom();
        draw_filter();
    }

    void PostProcessingController::prepare_bloom_effect(unsigned int wHeight, unsigned int wWidth) {
        if (m_hdrFBO == 0) {
            m_hdrFBO = Framebuffer::generate_framebuffer();
            Framebuffer::bind_framebuffer(m_hdrFBO);

            std::tie(m_colorBuffers[0], m_colorBuffers[1]) = Framebuffer::generate_two_framebuffer_textures();
            for (unsigned int i = 0; i < 2; i++) {
                Framebuffer::set_up_framebuffer_texture(wWidth, wHeight, m_colorBuffers[i], i);
            }
            Framebuffer::generate_depth_buffer(wWidth, wHeight);
            Framebuffer::set_up_attachments();
            Framebuffer::bind_framebuffer(0);

            std::tie(m_pingpongFBO[0], m_pingpongFBO[1])       = Framebuffer::generate_two_framebuffers();
            std::tie(m_pingpongBuffer[0], m_pingpongBuffer[1]) = Framebuffer::generate_two_framebuffer_textures();
            for (unsigned int i = 0; i < 2; i++) {
                Framebuffer::bind_framebuffer(m_pingpongFBO[i]);
                Framebuffer::set_up_framebuffer_texture(wWidth, wHeight, m_pingpongBuffer[i], 0);
            }
            prepare_bloom_shaders();
        }
    }

    void PostProcessingController::prepare_filter_effect(unsigned int wHeight, unsigned int wWidth) {
        if (m_screenFBO == 0) {
            m_screenFBO = Framebuffer::generate_framebuffer();
            Framebuffer::bind_framebuffer(m_screenFBO);

            m_screenTexture = Framebuffer::generate_framebuffer_texture();
            Framebuffer::set_up_framebuffer_texture(wWidth, wHeight, m_screenTexture, 0);

            Framebuffer::bind_framebuffer(0);
        }
    }

    void PostProcessingController::prepare_bloom_shaders() {
        auto resources                 = get<resources::ResourcesController>();
        resources::Shader *bloom       = resources->shader("bloom");
        resources::Shader *blur        = resources->shader("blur");
        resources::Shader *bloom_final = resources->shader("bloom_final");

        bloom->use();
        bloom->set_int("diffuseTexture", 0);
        blur->use();
        blur->set_int("image", 0);
        bloom_final->use();
        bloom_final->set_int("scene", 0);
        bloom_final->set_int("bloomBlur", 1);
    }

    void PostProcessingController::begin_draw() {
        Framebuffer::bind_framebuffer(m_hdrFBO);
        OpenGL::clear_buffers();
    }

    void PostProcessingController::draw_bloom() {
        auto resources                 = get<resources::ResourcesController>();
        resources::Shader *bloom_final = resources->shader("bloom_final");
        resources::Shader *blur        = resources->shader("blur");

        Framebuffer::bind_framebuffer(0);

        bool horizontal     = true, first_iteration = true;
        unsigned int amount = 10;
        blur->use();
        for (unsigned int i = 0; i < amount; i++) {
            Framebuffer::bind_framebuffer(m_pingpongFBO[horizontal]);
            blur->set_int("horizontal", horizontal);
            Framebuffer::bind_texture(first_iteration ? m_colorBuffers[1] : m_pingpongBuffer[!horizontal]);
            Framebuffer::render_quad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        Framebuffer::bind_framebuffer(0);

        Framebuffer::bind_framebuffer(m_screenFBO);
        OpenGL::clear_buffers(false);
        bloom_final->use();
        Framebuffer::activate_texture(m_colorBuffers[0], 0);
        Framebuffer::activate_texture(m_pingpongBuffer[!horizontal], 1);
        bloom_final->set_int("bloom", true);
        bloom_final->set_float("exposure", 1.0f);
        Framebuffer::render_quad();

        Framebuffer::bind_framebuffer(0);
    }

    void PostProcessingController::draw_filter() {
        auto resources                            = get<resources::ResourcesController>();
        resources::Shader *post_processing_shader = resources->shader(m_active_filter);
        post_processing_shader->use();

        Framebuffer::bind_framebuffer(0);
        OpenGL::clear_buffers(false);
        Framebuffer::activate_texture(m_screenTexture, 0);

        Framebuffer::render_quad();
    }

    void PostProcessingController::terminate() {
        if (m_hdrFBO != 0) {
            Framebuffer::delete_framebuffer(m_hdrFBO);
            m_hdrFBO = 0;
        }
        if (m_screenFBO != 0) {
            Framebuffer::delete_framebuffer(m_screenFBO);
            m_screenFBO = 0;
        }
        for (unsigned int i = 0; i < 2; i++) {
            if (m_pingpongFBO[i] != 0) {
                Framebuffer::delete_framebuffer(m_pingpongFBO[i]);
                m_pingpongFBO[i] = 0;
            }
        }
        if (m_colorBuffers[0] != 0 || m_colorBuffers[1] != 0) {
            OpenGL::delete_texture(m_colorBuffers[0]);
            m_colorBuffers[0] = m_colorBuffers[1] = 0;
        }
        if (m_pingpongBuffer[0] != 0 || m_pingpongBuffer[1] != 0) {
            OpenGL::delete_texture(m_pingpongBuffer[0]);
            OpenGL::delete_texture(m_pingpongBuffer[1]);
            m_pingpongBuffer[0] = m_pingpongBuffer[1] = 0;
        }
        if (m_screenTexture != 0) {
            OpenGL::delete_texture(m_screenTexture);
            m_screenTexture = 0;
        }
        Framebuffer::delete_quad();
    }

    std::string PostProcessingController::get_active_filter() const {
        return m_active_filter;
    }

    void PostProcessingController::set_active_filter(std::string filterShader) {
        m_active_filter = filterShader;
    }
}
