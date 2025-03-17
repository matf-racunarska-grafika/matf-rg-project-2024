#include <engine/core/Controller.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/PostProcessingController.hpp>
#include <spdlog/spdlog.h>

namespace engine::graphics {
    static std::array<std::string, FILTER_COUNT> m_filter_name_key_map;

    void initialize_filter_name_key_map();

    void PostProcessingController::initialize() {
        auto platform              = get<platform::PlatformController>();
        auto window                = platform->window();
        unsigned int window_height = window->height();
        unsigned int window_width  = window->width();

        prepare_bloom_effect(window_height, window_width);
        prepare_filter_effect(window_height, window_width);
        Framebuffer::set_up_quad();

        initialize_filter_name_key_map();
    }

    void PostProcessingController::draw() {
        draw_bloom();
        draw_filter();
    }

    void PostProcessingController::prepare_bloom_effect(unsigned int window_height, unsigned int window_width) {
        if (m_hdrFBO == 0) {
            m_hdrFBO = Framebuffer::generate_framebuffer();
            Framebuffer::bind_framebuffer(m_hdrFBO);

            std::tie(m_color_buffers[0], m_color_buffers[1]) = Framebuffer::generate_two_framebuffer_textures();
            for (unsigned int i = 0; i < 2; i++) {
                Framebuffer::set_up_framebuffer_texture(window_width, window_height, m_color_buffers[i], i);
            }
            Framebuffer::generate_depth_buffer(window_width, window_height);
            Framebuffer::set_up_attachments();
            Framebuffer::bind_framebuffer(0);

            std::tie(m_pingpongFBO[0], m_pingpongFBO[1])         = Framebuffer::generate_two_framebuffers();
            std::tie(m_pingpong_buffer[0], m_pingpong_buffer[1]) = Framebuffer::generate_two_framebuffer_textures();
            for (unsigned int i = 0; i < 2; i++) {
                Framebuffer::bind_framebuffer(m_pingpongFBO[i]);
                Framebuffer::set_up_framebuffer_texture(window_width, window_height, m_pingpong_buffer[i], 0);
            }
            prepare_bloom_shaders();
        }
    }

    void PostProcessingController::prepare_filter_effect(unsigned int window_height, unsigned int window_width) {
        if (m_screenFBO == 0) {
            m_screenFBO = Framebuffer::generate_framebuffer();
            Framebuffer::bind_framebuffer(m_screenFBO);

            m_screen_texture = Framebuffer::generate_framebuffer_texture();
            Framebuffer::set_up_framebuffer_texture(window_width, window_height, m_screen_texture, 0);

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
            Framebuffer::bind_texture(first_iteration ? m_color_buffers[1] : m_pingpong_buffer[!horizontal]);
            Framebuffer::render_quad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        Framebuffer::bind_framebuffer(0);

        Framebuffer::bind_framebuffer(m_screenFBO);
        OpenGL::clear_buffers(false);
        bloom_final->use();
        Framebuffer::activate_texture(m_color_buffers[0], 0);
        Framebuffer::activate_texture(m_pingpong_buffer[!horizontal], 1);
        bloom_final->set_int("bloom", true);
        bloom_final->set_float("exposure", 1.0f);
        Framebuffer::render_quad();

        Framebuffer::bind_framebuffer(0);
    }

    void PostProcessingController::draw_filter() {
        auto resources                            = get<resources::ResourcesController>();
        resources::Shader *post_processing_shader = resources->shader(
                m_filter_name_key_map[static_cast<int>(m_active_filter)]);
        post_processing_shader->use();

        Framebuffer::bind_framebuffer(0);
        OpenGL::clear_buffers(false);
        Framebuffer::activate_texture(m_screen_texture, 0);

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
        if (m_color_buffers[0] != 0 || m_color_buffers[1] != 0) {
            OpenGL::delete_texture(m_color_buffers[0]);
            m_color_buffers[0] = m_color_buffers[1] = 0;
        }
        if (m_pingpong_buffer[0] != 0 || m_pingpong_buffer[1] != 0) {
            OpenGL::delete_texture(m_pingpong_buffer[0]);
            OpenGL::delete_texture(m_pingpong_buffer[1]);
            m_pingpong_buffer[0] = m_pingpong_buffer[1] = 0;
        }
        if (m_screen_texture != 0) {
            OpenGL::delete_texture(m_screen_texture);
            m_screen_texture = 0;
        }
        Framebuffer::delete_quad();
    }

    Filter PostProcessingController::get_active_filter() const {
        return m_active_filter;
    }

    void PostProcessingController::set_active_filter(Filter filter) {
        m_active_filter = filter;
    }

    void initialize_filter_name_key_map() {
#include "post_processing_filter_key_to_string.include"
    }
}
