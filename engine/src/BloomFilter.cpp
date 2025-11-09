//
// Created by matija on 10/29/25.
//


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
    destroy_buffers();
}

float engine::graphics::BloomFilter::threshold() const {
    return m_bright_threshold;
}
void engine::graphics::BloomFilter::initialize_buffers(unsigned int scr_width, unsigned int scr_height) {
    destroy_buffers();
    m_bloom_buffer.init(scr_width, scr_height);
    m_bloom_buffer.add_color_attachment(FrameTextureType::FLOAT,true,"image");
    m_bloom_buffer.add_color_attachment(FrameTextureType::FLOAT,true,"bloom");


    m_pong_buffer[0].init(scr_width, scr_height,false);
    m_pong_buffer[0].add_color_attachment(FrameTextureType::FLOAT,true,"blur");

    m_pong_buffer[1].init(scr_width, scr_height,false);
    m_pong_buffer[1].add_color_attachment(FrameTextureType::FLOAT,true,"blur");
}

void engine::graphics::BloomFilter::set_up_canvas() {
    m_bloom_buffer.bind();
}

void engine::graphics::BloomFilter::apply_bloom() {
    m_combine_shader->use();
    
    engine::graphics::OpenGL::bind_texture(m_bloom_buffer.texture("image"),0);

    engine::graphics::OpenGL::bind_texture(m_pong_buffer[0].texture("blur"),1);

    m_combine_shader->set_float("intensity",m_intensity);
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_quad();
}

void engine::graphics::BloomFilter::clear_buffers() {
    m_bloom_buffer.clear();
    m_pong_buffer[0].clear();
    m_pong_buffer[1].clear();
}

void engine::graphics::BloomFilter::apply_blur() {

    m_blur_shader_v->use();
    m_pong_buffer[0].bind();
    engine::graphics::OpenGL::bind_texture(m_bloom_buffer.texture("bloom"),0);
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
        engine::graphics::OpenGL::bind_texture(m_pong_buffer[i%2].texture("blur"),0);
        engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_quad();
    }
    engine::graphics::OpenGL::bind_frame_buffer(0);
}

unsigned int engine::graphics::BloomFilter::framebuffer_id() {
    return m_bloom_buffer.framebuffer_id();
}

void engine::graphics::BloomFilter::set_threshold(float threshold) {
    m_bright_threshold=threshold;
}

void engine::graphics::BloomFilter::set_intensity(float brightness) {
    m_intensity=brightness;
}

void engine::graphics::BloomFilter::destroy_buffers() {
    m_pong_buffer[0].free();
    m_pong_buffer[1].free();
    m_bloom_buffer.free();
}