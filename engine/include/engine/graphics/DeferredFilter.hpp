//
// Created by matija on 10/29/25.
//

#ifndef MATF_RG_PROJECT_DEFERREDFILTER_HPP
#define MATF_RG_PROJECT_DEFERREDFILTER_HPP

#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/MyFrameBuffer.hpp>

namespace engine::graphics {

class DeferredFilter {
public:
    DeferredFilter();
    ~DeferredFilter();

    void initilizeBuffers(unsigned int scr_width, unsigned int scr_height);

    void setUpCanvas();

    void render(resources::Shader shader);

    void blitDepth(unsigned int width, unsigned int height, unsigned int toFbo) const;

private:
    GraphicsController* graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    resources::MyFrameBuffer m_gbuffer;
    resources::Shader* m_geometry_shader = nullptr; // writes to G-Buffer

    void destroyBuffers();
};
}

#endif //MATF_RG_PROJECT_DEFERREDFILTER_HPP