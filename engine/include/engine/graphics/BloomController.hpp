#ifndef BLOOMCONTROLLER_HPP
#define BLOOMCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>

namespace engine::graphics {
    class BloomController final : public core::Controller {
    public:
        unsigned int pingpong_FBO[2];
        unsigned int pingpong_colorbuffers[2];
        unsigned int hdr_FBO;
        unsigned int color_buffers[2];
        unsigned int scr_width  = get<platform::PlatformController>()->window()->width();
        unsigned int scr_height = get<platform::PlatformController>()->window()->height();
        int bloom_passes              = 10;
        float exposure                = 1.0f;
        bool bloom                    = true;
        float bloom_strength           = 1.0f;

        void render_bloom();

        void prepare_hdr();

        void finalize_bloom();

        void hdr_bloom_setup();

        [[nodiscard]] std::string_view name() const override {
            return "app::BloomController";
        }

    private:
        unsigned int m_quadVAO = 0;
        unsigned int m_quadVBO = 0;

        void render_quad();
    };
}

#endif //BLOOMCONTROLLER_HPP
