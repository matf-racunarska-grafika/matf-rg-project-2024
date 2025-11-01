//
// Created by zpantovic on 12.10.25..
//

#ifndef BLOOMCONTROLLER_HPP
#define BLOOMCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>

namespace engine::graphics {

class BloomController final : public core::Controller {
public:
    int bloom_passes = 10;
    float exposure = 0.8f;
    bool bloom = true;
    float bloom_strength = 0.8f;

    void toggle_bloom();
    void set_bloom(bool enabled);
    bool is_bloom_enabled() const;

    void render_bloom();
    void prepare_hdr();
    void finalize_bloom();
    void bloom_setup();

    [[nodiscard]] std::string_view name() const override {
        return "app::BloomController";
    }

    void terminate() override;

private:
    unsigned int m_pingpong_fbo[2] = {};
    unsigned int m_pingpong_colorbuffers[2] = {};
    unsigned int m_hdr_fbo = 0;
    unsigned int m_color_buffers[2] = {};
    unsigned int m_scr_width = 0;
    unsigned int m_scr_height = 0;
    unsigned int m_quad_vao = 0;
    unsigned int m_quad_vbo = 0;

    void render_quad();
};

} // namespace engine::graphics

#endif // BLOOMCONTROLLER_HPP


