//
// Created by matija on 10/29/25.
//

#include <engine/resources/MyFrameBuffer.hpp>
using namespace engine::resources;

MyFrameBuffer::MyFrameBuffer() {}

MyFrameBuffer::~MyFrameBuffer() {
    free();
}

void MyFrameBuffer::init(int width, int height, bool use_depth) {
    this->m_width = width;
    this->m_height = height;

    m_fbo = engine::graphics::OpenGL::gen_framebuffer();
    if (use_depth)
        m_depth_buffer = graphics::OpenGL::add_render_buffer(m_fbo, width, height);
}

void MyFrameBuffer::add_color_attachment(graphics::FrameTextureType type, bool linear, const std::string &name) {
    unsigned int tex = graphics::OpenGL::add_frame_texture(m_fbo,  texture_count(), type, m_width, m_height, linear);
    m_color_textures[name] = tex;
    graphics::OpenGL::set_attachment_count(m_fbo, texture_count());
}

void MyFrameBuffer::bind() const {
    graphics::OpenGL::bind_frame_buffer(m_fbo);
}

void MyFrameBuffer::clear() {
    graphics::OpenGL::clear_buffers();
}

void MyFrameBuffer::unbind() {
    graphics::OpenGL::bind_frame_buffer(0);
}

void MyFrameBuffer::bind_texture(std::string name, unsigned int slot) {
    graphics::OpenGL::bind_texture(m_color_textures[name], slot);
}


void MyFrameBuffer::free() {
    if (m_depth_buffer) {
        graphics::OpenGL::delete_render_buffer(m_depth_buffer);
        m_depth_buffer = 0;
    }

    if (!m_color_textures.empty()) {
        for (auto pair: m_color_textures) {
            graphics::OpenGL::delete_texture(pair.second);
        }
        m_color_textures.clear();
    }

    if (m_fbo) {
        graphics::OpenGL::delete_frame_buffer(m_fbo);
        m_fbo = 0;
    }
}
