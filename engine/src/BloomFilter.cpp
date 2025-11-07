//
// Created by matija on 10/29/25.
//

#include "../include/engine/graphics/BloomFilter.hpp"

#include <iostream>
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

float engine::graphics::BloomFilter::getTreshold() const {
    return m_bright_threshold;
}
void engine::graphics::BloomFilter::initilizeBuffers(unsigned int scr_width, unsigned int scr_height) {
    destroyBuffers();
    m_bloom_buffer.init(scr_width, scr_height);
    m_bloom_buffer.addColorAttachment(FrameTextureType::FLOAT,true,"image");
    m_bloom_buffer.addColorAttachment(FrameTextureType::FLOAT,true,"bloom");


    m_pong_buffer[0].init(scr_width, scr_height,false);
    m_pong_buffer[0].addColorAttachment(FrameTextureType::FLOAT,true,"blur");

    m_pong_buffer[1].init(scr_width, scr_height,false);
    m_pong_buffer[1].addColorAttachment(FrameTextureType::FLOAT,true,"blur");



}

void engine::graphics::BloomFilter::setUpCanvas() {
    m_bloom_buffer.bind();
}

void engine::graphics::BloomFilter::applyBloom() {
    m_combine_shader->use();
    
    engine::graphics::OpenGL::BindTexture(m_bloom_buffer.getTexture("image"),0);

    engine::graphics::OpenGL::BindTexture(m_pong_buffer[0].getTexture("blur"),1);

    m_combine_shader->set_float("intensity",m_intensity);
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_quad();
}

void engine::graphics::BloomFilter::clearBuffers() {
    m_bloom_buffer.clear();
    m_pong_buffer[0].clear();
    m_pong_buffer[1].clear();
}

void engine::graphics::BloomFilter::applyBlur() {

    m_blur_shader_v->use();
    m_pong_buffer[0].bind();
    engine::graphics::OpenGL::BindTexture(m_bloom_buffer.getTexture("bloom"),0);
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_quad();

    for (unsigned int i = 0; i < 2*m_num_swaps; i++)
    {
        m_pong_buffer[(i+1)%2].bind();
        if (i%2 == 0) {
            m_blur_shader_h->use();
        }
        else {
            m_blur_shader_v->use();
        }
        engine::graphics::OpenGL::BindTexture(m_pong_buffer[i%2].getTexture("blur"),0);
        engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_quad();
    }
    engine::graphics::OpenGL::bindFrameBuffer(0);
}

unsigned int engine::graphics::BloomFilter::get_framebuffer_id() {
    return m_bloom_buffer.getFrameBuff();
}

void engine::graphics::BloomFilter::set_threshold(float threshold) {
    m_bright_threshold=threshold;
}

void engine::graphics::BloomFilter::setIntensity(float brightness) {
    m_intensity=brightness;
}

void engine::graphics::BloomFilter::destroyBuffers() {
    m_pong_buffer[0].free();
    m_pong_buffer[1].free();
    m_bloom_buffer.free();
}