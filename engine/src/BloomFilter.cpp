//
// Created by matija on 10/29/25.
//

#include "../include/engine/graphics/BloomFilter.hpp"

#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/ResourcesController.hpp>

engine::graphics::BloomFilter::BloomFilter(float intensity, float threshold, unsigned int num_of_passes) : m_intensity(intensity) ,m_bright_threshold(threshold), m_num_swaps(num_of_passes) {
    m_combine_shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("combine_shader");
    m_blur_shader_v = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("blur_shader_v");
    m_blur_shader_h = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("blur_shader_h");

    m_combine_shader->use();
    m_combine_shader->set_int("image", 0);
    m_combine_shader->set_int("bloom", 1);

    m_blur_shader_v->use();
    m_blur_shader_v->set_int("image",0);

    m_blur_shader_h->use();
    m_blur_shader_h->set_int("image",0);

}

engine::graphics::BloomFilter::~BloomFilter() {
    destroyBuffers();
}

void engine::graphics::BloomFilter::initilizeBuffers(unsigned int scr_width, unsigned int scr_height) {
    m_bloom_fbo= engine::graphics::OpenGL::makeBloomFramebuffer(scr_width, scr_height);
    m_blur_pongs[0]=engine::graphics::OpenGL::makeSimpleColorBuffer(scr_width, scr_height);
    m_blur_pongs[1]=engine::graphics::OpenGL::makeSimpleColorBuffer(scr_width, scr_height);
}

void engine::graphics::BloomFilter::setUpCanvas() {
    engine::graphics::OpenGL::ActivateBuffer(m_bloom_fbo.fbo);
}

void engine::graphics::BloomFilter::applyBloom() {
    m_combine_shader->use();
    engine::graphics::OpenGL::SetTextureSlot(0);
    engine::graphics::OpenGL::BindTexture(m_bloom_fbo.texture_normal);

    engine::graphics::OpenGL::SetTextureSlot(1);
    engine::graphics::OpenGL::BindTexture(m_blur_pongs[0].fbo);

    m_combine_shader->set_float("intensity",m_intensity);
    graphcis->draw_quad();

}

void engine::graphics::BloomFilter::applyBlur() {

    m_blur_shader_v->use();
    engine::graphics::OpenGL::ActivateBuffer(m_blur_pongs[0].fbo);
    engine::graphics::OpenGL::BindTexture(m_bloom_fbo.texture_bright);
    graphcis->draw_quad();

    for (unsigned int i = 0; i < m_num_swaps; i++)
    {
        engine::graphics::OpenGL::ActivateBuffer(m_blur_pongs[(i+1)%2].fbo);
        if (i%2 == 0) {
            m_blur_shader_h->use();
        }
        else {
            m_blur_shader_v->use();
        }

        engine::graphics::OpenGL::BindTexture(m_blur_pongs[i%2].fbo);
        graphcis->draw_quad();
    }
    engine::graphics::OpenGL::ActivateBuffer(0);
}

void engine::graphics::BloomFilter::destroyBuffers() {
    engine::graphics::OpenGL::DestroyBuffer(m_bloom_fbo);
    engine::graphics::OpenGL::DestroyBuffer(m_blur_pongs[0]);
    engine::graphics::OpenGL::DestroyBuffer(m_blur_pongs[1]);
    m_bloom_fbo={};
    m_blur_pongs[0]={};
    m_blur_pongs[1]={};
}