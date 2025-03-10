/**
* @file PostProcessingController.hpp
* @brief Defines the PostProcessingController class that manages resources and rendering for bloom and filter effects.
*/
#ifndef POSTPROCESSINGCONTROLLER_HPP
#define POSTPROCESSINGCONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace engine::graphics {
  class PostProcessingController : public engine::core::Controller {
    unsigned int m_hdrFBO            = 0;
    unsigned int m_colorBuffers[2]   = {0, 0};
    unsigned int m_pingpongFBO[2]    = {0, 0};
    unsigned int m_pingpongBuffer[2] = {0, 0};
    unsigned int m_screenFBO         = 0;
    unsigned int m_screenTexture     = 0;

    std::string m_active_filter = "noFilter";

  public:
    /**
     * @brief calls all the functions to prepare resources for rendering bloom and filters
     */
    void initialize() override;

    /**
     * @brief prepares the final framebuffer that will be rendered to the screen
     * @param wHeight height of the screen
     * @param wWidth width of the screen
     */
    void prepare_filter_effect(unsigned int wHeight, unsigned int wWidth);

    /**
     * @brief sets up the hdr framebuffer and color and pingpong buffers for bloom
     * @param wHeight height of the screen
     * @param wWidth width of the screen
     */
    void prepare_bloom_effect(unsigned int wHeight, unsigned int wWidth);

    /**
     * @brief sets up uniforms for default bloom shaders
     */
    void prepare_bloom_shaders();

    /**
     * @brief a getter for the name of the active filter
     * @return the name of the active filter shader
     */
    std::string get_active_filter() const;

    /**
     * @brief a setter for the name of the active filter
     * @param filterShader the name of the filter shader that should be activated
     */
    void set_active_filter(std::string filterShader);

    /**
     * @brief draws all post-processing effects into framebuffers
     */
    void draw() override;

    /**
     * @brief binds the appropriate framebuffer and clears buffers
     */
    void begin_draw() override;

    /**
     * @brief applies the bloom effect
     */
    void draw_bloom();

    /**
     * @brief applies the current active filter
     */
    void draw_filter();

    /**
     * @brief deletes all framebuffers and related textures
     */
    void terminate() override;

    /**
     * @brief helps to easily identify the controller in logs by giving it a name
     * @return name of the Controller class
     */
    std::string_view name() const override {
      return "PostProcessingController";
    }
  };
}

#endif //POSTPROCESSINGCONTROLLER_HPP
