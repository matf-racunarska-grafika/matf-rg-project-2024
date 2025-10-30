//
// Created by matija on 10/29/25.
//

#include "../include/engine/graphics/DeferredFilter.hpp"

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
    destroyBuffers();
}

void DeferredFilter::initilizeBuffers(unsigned int scr_width, unsigned int scr_height) {
    destroyBuffers();
    m_gbuffer.init(scr_width, scr_height, true);
    m_gbuffer.addColorAttachment(FrameTextureType::FLOAT, false, "position");
    m_gbuffer.addColorAttachment(FrameTextureType::FLOAT, false, "normal");
    m_gbuffer.addColorAttachment(FrameTextureType::RGBA, false, "albedo_spec");
    m_gbuffer.addColorAttachment(FrameTextureType::RGBA, false, "emissive_shine");
}

void DeferredFilter::setUpCanvas(std::vector<Light>& lights) {
    m_gbuffer.bind();
    m_geometry_shader->use();
    m_geometry_shader->prepare_for_use();
    m_geometry_shader->set_lights(lights);
    engine::core::Controller::get<GraphicsController>()->prepare_for_draw(m_geometry_shader);

}



void DeferredFilter::render(resources::Shader* shader) {
    shader->use();
    shader->prepare_for_use();

    shader->set_int("gPosition", 0);
    shader->set_int("gNormal", 1);
    shader->set_int("gAlbedoSpec", 2);
    shader->set_int("gEmissiveShine", 3);

    m_gbuffer.bindTexture("position", 0);
    m_gbuffer.bindTexture("normal", 1);
    m_gbuffer.bindTexture("albedo_spec", 2);
    m_gbuffer.bindTexture("emissive_shine", 3);

    engine::core::Controller::get<GraphicsController>()->draw_quad();
}

void DeferredFilter::blitDepth(unsigned int width, unsigned int height ,unsigned int toFbo) const {
    graphics::OpenGL::BlitFrameBuffer(m_gbuffer.getFrameBuff(), toFbo, width, height, GL_DEPTH_BUFFER_BIT);
}
void DeferredFilter::clearBuffers() {
    m_gbuffer.clear();
}

void DeferredFilter::destroyBuffers() {
    m_gbuffer.free();
}
