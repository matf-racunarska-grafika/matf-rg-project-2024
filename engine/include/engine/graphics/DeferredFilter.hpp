//
// Created by matija on 10/29/25.
//

#ifndef MATF_RG_PROJECT_DEFERREDFILTER_HPP
#define MATF_RG_PROJECT_DEFERREDFILTER_HPP

#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/MyFrameBuffer.hpp>

namespace engine::graphics {

class DeferredFilter {
public:
    DeferredFilter();
    ~DeferredFilter();

    void initilizeBuffers(unsigned int scr_width, unsigned int scr_height);

    void setUpCanvas();

    void render(resources::Shader* shader);

    void blitDepth(unsigned int width, unsigned int height, unsigned int toFbo) const;

    void clearBuffers();
    void destroyBuffers();
    resources::Shader * geometry_shader() const { return m_geometry_shader; }

private:

    resources::MyFrameBuffer m_gbuffer;
    resources::Shader* m_geometry_shader = nullptr;

};
}

#endif //MATF_RG_PROJECT_DEFERREDFILTER_HPP