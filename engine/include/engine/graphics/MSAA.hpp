#ifndef ENGINE_GRAPHICS_MSAA_HPP
#define ENGINE_GRAPHICS_MSAA_HPP

#include <cstdint>

namespace engine::graphics {

/**
 * @class MSAA
 * @brief Off-screen multisample anti-aliasing helper.
 */
class MSAA {
public:
    /**
     * @param width     Framebuffer width.
     * @param height    Framebuffer height.
     * @param samples   Broj MSAA uzoraka (npr. 4).
     */
    MSAA(uint32_t width, uint32_t height, uint32_t samples = 4);

    ~MSAA();

    /// Bind-uje MSAA FBO za renderovanje.
    void bindForWriting();

    /// Resolve (blit) iz MSAA FBO u default framebuffer.
    void resolveToDefault();

private:
    uint32_t _fbo;
    uint32_t _colorRbo;
    uint32_t _depthRbo;
    uint32_t _width;
    uint32_t _height;
    uint32_t _samples;
};

}// namespace engine::graphics

#endif // ENGINE_GRAPHICS_MSAA_HPP
