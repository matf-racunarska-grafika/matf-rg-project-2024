#include "engine/graphics/MSAA.hpp"
#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace engine::graphics {

MSAA::MSAA(uint32_t width, uint32_t height, uint32_t samples)
: _width(width)
, _height(height)
, _samples(samples) {
    // 1) Kreiraj i bind-uj FBO
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    // 2) Color renderbuffer multisample
    glGenRenderbuffers(1, &_colorRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, GL_RGBA8, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRbo);

    // 3) Depth-stencil renderbuffer multisample
    glGenRenderbuffers(1, &_depthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, GL_DEPTH24_STENCIL8, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthRbo);

    // 4) Provera kompleksnosti
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { spdlog::error("MSAA FBO is not complete!"); }

    // 5) Vrati default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MSAA::~MSAA() {
    glDeleteRenderbuffers(1, &_depthRbo);
    glDeleteRenderbuffers(1, &_colorRbo);
    glDeleteFramebuffers(1, &_fbo);
}

void MSAA::bindForWriting() {
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void MSAA::resolveToDefault() {
    // Blit sa MSAA FBO u default framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
            0, 0, _width, _height,
            0, 0, _width, _height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST
            );
    // Vrati default
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}// namespace engine::graphics
