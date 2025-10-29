//
// Created by matija on 10/29/25.
//

#include "../include/engine/graphics/DeferredFilter.hpp"

#include <glad/glad.h>
#include <engine/resources/ResourcesController.hpp>

using namespace engine::graphics;
using namespace engine::resources;

DeferredFilter::DeferredFilter() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    m_geometry_shader = resources->shader("deferred_geometry");
    m_gbuffer=resources::MyFrameBuffer();
}

DeferredFilter::~DeferredFilter() {
    destroyBuffers();
}

void DeferredFilter::initilizeBuffers(unsigned int scr_width, unsigned int scr_height) {
    m_gbuffer.init(scr_width, scr_height, true);
    m_gbuffer.addColorAttachment(FrameTextureType::FLOAT, false, "position");
    m_gbuffer.addColorAttachment(FrameTextureType::FLOAT, false, "normal");
    m_gbuffer.addColorAttachment(FrameTextureType::RGBA, false, "albedo_spec");
}

void DeferredFilter::setUpCanvas() {
    m_gbuffer.bind();
    m_geometry_shader->use();
}



void DeferredFilter::render(resources::Shader shader) {
    shader.use();

    shader.set_int("gPosition", 0);
    shader.set_int("gNormal", 1);
    shader.set_int("gAlbedoSpec", 2);

    // Bind textures to the matching units
    m_gbuffer.bindTexture("position", 0);
    m_gbuffer.bindTexture("normal", 1);
    m_gbuffer.bindTexture("albedo_spec", 2);

    graphics->draw_quad();
}

void DeferredFilter::blitDepth(unsigned int width, unsigned int height ,unsigned int toFbo) const {
    graphics::OpenGL::BlitFrameBuffer(m_gbuffer.getFrameBuff(), toFbo, width, height, GL_DEPTH_BUFFER_BIT);
}

void DeferredFilter::destroyBuffers() {
    m_gbuffer.free();
}
