/**
* @file PostProcessingController.hpp
* @brief Defines the PostProcessingController class that manages resources and rendering for bloom and filter effects.
*/
#ifndef POSTPROCESSINGCONTROLLER_HPP
#define POSTPROCESSINGCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#define FILTER_COUNT 6

namespace engine::graphics {
  enum class Filter : int {
    NOFILTER   = 0,
    BLACKWHITE = 1,
    GRAYSCALE  = 2,
    NEGATIVE   = 3,
    OUTLINE    = 4,
    DEEPFRIED  = 5
  };

  class PostProcessingController : public engine::core::Controller {
    unsigned int m_hdrFBO            = 0;
    unsigned int m_colorBuffers[2]   = {0, 0};
    unsigned int m_pingpongFBO[2]    = {0, 0};
    unsigned int m_pingpongBuffer[2] = {0, 0};
    unsigned int m_screenFBO         = 0;
    unsigned int m_screenTexture     = 0;

    Filter m_active_filter = Filter::NOFILTER;

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
     * @brief a getter for the active filter
     * @return the currently active filter
     */
    Filter get_active_filter() const;

    /**
     * @brief a setter for the active filter
     * @param filter filter that should be activated from Filter enum class
     */
    void set_active_filter(Filter filter);

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
