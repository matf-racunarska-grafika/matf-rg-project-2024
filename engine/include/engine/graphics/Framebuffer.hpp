//
// Created by filipn on 10/9/25.
//

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#include <glm/fwd.hpp>

namespace engine::graphics {

class Framebuffer {
    friend class GraphicsController;

public:
    Framebuffer(): m_quadVAO(0)
               , m_fbo(0)
               , m_textureColorBufferMultiSampled(0)
               , m_rbo(0)
               , m_intermediateFBO(0)
               , m_screenTexture(0) {}

private:
    uint32_t m_quadVAO;

    uint32_t m_fbo;
    uint32_t m_textureColorBufferMultiSampled;
    uint32_t m_rbo;

    uint32_t m_intermediateFBO;
    uint32_t m_screenTexture;
};

}

#endif //FRAMEBUFFER_HPP
