#ifndef BLOOMCONTROLLER_HPP
#define BLOOMCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace engine::graphics {
    class BloomController final : public core::Controller {
    public:
        unsigned int pingpongFBO[2];
        unsigned int pingpongColorbuffers[2];
        unsigned int hdrFBO;
        unsigned int colorBuffers[2];
        const unsigned int SCR_WIDTH  = 1400;
        const unsigned int SCR_HEIGHT = 1000;
        int bloom_passes = 10;
        float exposure                = 1.0f;
        bool bloom                    = true;
        float bloomStrength = 1.0f;
        void render_bloom();
        void prepare_hdr();
        void finalize_bloom();
        void hdr_bloom_setup();
    private:
        unsigned int m_quadVAO = 0;
        unsigned int m_quadVBO = 0;
        void renderQuad();
    };
} // namespace engine::graphics

#endif //BLOOMCONTROLLER_HPP
