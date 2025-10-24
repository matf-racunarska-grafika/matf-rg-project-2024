//
// Created by nikola on 10/23/25.
//

#ifndef MATF_RG_PROJECT_BLOOMCONTROLLER_HPP
#define MATF_RG_PROJECT_BLOOMCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/resources/Shader.hpp>

namespace app {
    class BloomController: public engine::core::Controller {
        unsigned int m_pingpong_fbo[2] = { 0, 0 };
        unsigned int m_color_buffers[2] = { 0, 0 };
        unsigned int m_pingpong_color_buffers[2] = { 0, 0 };
        unsigned int m_hdr_fbo = 0;
        float m_exposure = 2.0f;
        unsigned int m_quad_vao = 0;
        unsigned int m_quad_vbo = 0;
        engine::resources::Shader* m_shader_blur = nullptr;
        engine::resources::Shader* m_shader_bloom_final = nullptr;
        void initialize() override;
        void renderQuad();

        void begin_draw() override;
        void draw() override;
    public:
        std::string_view name() const override { return "App::BloomController"; }
    };
} // app

#endif //MATF_RG_PROJECT_BLOOMCONTROLLER_HPP