//
// Created by matija on 10/29/25.
//


#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <glad/glad.h>

using namespace engine::graphics;
using namespace engine::resources;

DeferredFilter::DeferredFilter() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    m_geometry_shader = resources->shader("deferred_geometry");
    m_gbuffer=resources::MyFrameBuffer();
}

DeferredFilter::~DeferredFilter() {
    destroy_buffers();
}

void DeferredFilter::initialize_buffers(unsigned int scr_width, unsigned int scr_height) {
    destroy_buffers();
    m_gbuffer.init(scr_width, scr_height, true);
    m_gbuffer.add_color_attachment(FrameTextureType::FLOAT, false, "position");
    m_gbuffer.add_color_attachment(FrameTextureType::FLOAT, false, "normal");
    m_gbuffer.add_color_attachment(FrameTextureType::RGBA, false, "albedo_spec");
    m_gbuffer.add_color_attachment(FrameTextureType::RGBA, false, "emissive_shine");
}

void DeferredFilter::set_up_canvas() {
    m_gbuffer.bind();
    clear_buffers();
    m_geometry_shader->use();
    //m_geometry_shader->prepare_for_use();
    engine::core::Controller::get<GraphicsController>()->prepare_for_draw(m_geometry_shader);
}



void DeferredFilter::render(resources::Shader* shader) {
    shader->use();
    //shader->prepare_for_use();

    shader->set_int("gPosition", 0);
    shader->set_int("gNormal", 1);
    shader->set_int("gAlbedoSpec", 2);
    shader->set_int("gEmissiveShine", 3);

    m_gbuffer.bind_texture("position", 0);
    m_gbuffer.bind_texture("normal", 1);
    m_gbuffer.bind_texture("albedo_spec", 2);
    m_gbuffer.bind_texture("emissive_shine", 3);

    engine::core::Controller::get<GraphicsController>()->draw_quad();
}

void DeferredFilter::blit_depth(unsigned int width, unsigned int height ,unsigned int to_fbo) const {
    graphics::OpenGL::blit_framebuffer(m_gbuffer.framebuffer_id(), to_fbo, width, height, GL_DEPTH_BUFFER_BIT);
}
void DeferredFilter::clear_buffers() {
    m_gbuffer.clear();
}

void DeferredFilter::destroy_buffers() {
    m_gbuffer.free();
}
