//
// Created by filipn on 10/9/25.
//

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#include <glm/fwd.hpp>

namespace engine::graphics {

class Framebuffer {
    friend class GraphicsController;

private:
    static uint32_t m_quadVAO;

    static uint32_t m_fbo;
    static uint32_t m_textureColorBufferMultiSampled;
    static uint32_t m_rbo;

    static uint32_t m_intermediateFBO;
    static uint32_t m_screenTexture;
};

}

#endif //FRAMEBUFFER_HPP
