//
// Created by filipn on 10/9/25.
//

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#include <glm/fwd.hpp>

namespace engine::graphics {

class Framebuffer {
public:
    void init_msaa(uint32_t width, uint32_t height, uint32_t samples = 4);

    void bind() const;

    void unbind() const;

    void resolve() const;

private:
    uint32_t m_fbo{0};
    uint32_t m_textureColorBufferMultiSampled{0};
    uint32_t m_rbo{0};

    uint32_t m_intermediateFBO{0};
    uint32_t m_screenTexture{0};
};

}

#endif //FRAMEBUFFER_HPP
