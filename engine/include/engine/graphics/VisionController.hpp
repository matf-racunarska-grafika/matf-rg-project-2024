#ifndef MATF_RG_PROJECT_VISIONCONTROLLER_HPP
#define MATF_RG_PROJECT_VISIONCONTROLLER_HPP

#include <engine/graphics/OpenGL.hpp>
#include "engine/core/Controller.hpp"
#include <engine/graphics/GraphicsController.hpp>
#include <glm/glm.hpp>



namespace engine {
namespace vision {

class VisionController : public core::Controller {
public:
    VisionController() = default;
    ~VisionController() override;

    void initialize() override;
    void update() override;

    void begin_scene();
    void end_scene();

    void set_bloom_enabled(bool enabled) { m_bloom_enabled = enabled; }
    bool is_bloom_enabled() const { return m_bloom_enabled; }

    void toggle_bloom() { m_bloom_enabled = !m_bloom_enabled; }

    void set_bloom_threshold(float threshold) { m_bloom_threshold = threshold; }
    float get_bloom_threshold() const { return m_bloom_threshold; }

    void set_bloom_intensity(float intensity) { m_bloom_intensity = intensity; }
    float get_bloom_intensity() const { return m_bloom_intensity; }

    void set_blur_passes(int passes) { m_blur_passes = passes; }
    int get_blur_passes() const { return m_blur_passes; }

    void set_exposure(float exposure) { m_exposure = exposure; }
    float get_exposure() const { return m_exposure; }

private:
    void create_framebuffers();
    void cleanup_framebuffers();
    void resize_framebuffers(int width, int height);

    void render_bright_pass();
    void render_blur_pass();
    void render_composite();
    void render_quad();
    void render_passthrough();

    void load_shaders();


    uint32_t m_scene_fbo = 0;
    uint32_t m_scene_color_buffer = 0;
    uint32_t m_scene_depth_buffer = 0;

    uint32_t m_bright_fbo = 0;
    uint32_t m_bright_color_buffer = 0;

    uint32_t m_ping_pong_fbo[2] = {0, 0};
    uint32_t m_ping_pong_color_buffers[2] = {0, 0};

    uint32_t m_quad_vao = 0;
    uint32_t m_quad_vbo = 0;

    std::string m_bloom_shader = "bloom";

    bool m_bloom_enabled = true;
    float m_bloom_threshold = 1.0f;
    float m_bloom_intensity = 0.8f;
    int m_blur_passes = 5;
    float m_exposure = 1.0f;

    int m_width = 0;
    int m_height = 0;

    bool m_initialized = false;
    bool m_in_scene = false;

};


}
}
#endif//MATF_RG_PROJECT_VISIONCONTROLLER_HPP
