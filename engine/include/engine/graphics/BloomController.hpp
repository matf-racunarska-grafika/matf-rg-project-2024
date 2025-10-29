/**
 * @file BloomController.hpp
 * @brief Defines the BloomController class that handles bloom post-processing effect.
 */

#ifndef BLOOMCONTROLLER_HPP
#define BLOOMCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/resources/Shader.hpp>
#include <memory>

namespace engine::graphics {

/**
 * @class BloomController
 * @brief Handles bloom post-processing effect with HDR rendering and Gaussian blur.
 */
class BloomController final : public core::Controller {
public:
    std::string_view name() const override;

    /**
     * @brief Enable or disable bloom effect
     */
    void set_enabled(bool enabled) { m_enabled = enabled; }

    /**
     * @brief Set bloom intensity
     */
    void set_intensity(float intensity) { m_bloomIntensity = intensity; }

    /**
     * @brief Set bloom threshold for bright areas
     */
    void set_threshold(float threshold) { m_bloomThreshold = threshold; }

    /**
     * @brief Begin bloom rendering - should be called before scene rendering
     */
    void begin_frame();

    /**
     * @brief Apply bloom effect - should be called after scene rendering
     */
    void apply_bloom();

    /**
     * @brief Get whether bloom is enabled
     */
    bool is_enabled() const { return m_enabled; }

    /**
     * @brief Get current bloom intensity
     */
    float intensity() const { return m_bloomIntensity; }

private:
    void initialize() override;

    void terminate() override;

    void create_framebuffers();

    void create_quad();

    void render_quad();

    bool m_enabled = true;
    float m_bloomIntensity = 1.0f;
    float m_bloomThreshold = 1.0f;

    // Framebuffers
    uint32_t m_hdrFBO = 0;
    uint32_t m_pingpongFBO[2] = {0, 0};
    uint32_t m_colorBuffers[2] = {0, 0};
    uint32_t m_pingpongColorbuffers[2] = {0, 0};

    // Shaders
    std::unique_ptr<resources::Shader> m_bloomShader;
    std::unique_ptr<resources::Shader> m_blurShader;
    std::unique_ptr<resources::Shader> m_finalShader;

    // Render quad
    uint32_t m_quadVAO = 0;
    uint32_t m_quadVBO = 0;

    bool m_initialized = false;
};

}// namespace engine::graphics

#endif // BLOOMCONTROLLER_HPP