#ifndef POSTPROCESSINGCONTROLLER_HPP
#define POSTPROCESSINGCONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace engine::graphics {
  class PostProcessingController : public engine::core::Controller {
    unsigned int m_hdrFBO = 0;
    unsigned int m_colorBuffers[2] = { 0, 0 };
    unsigned int m_pingpongFBO[2] = { 0, 0 };
    unsigned int m_pingpongBuffer[2] = { 0, 0 };
    unsigned int m_screenFBO = 0;
    unsigned int m_screenTexture = 0;

    std::string m_active_filter = "noFilter";

  public:
    std::string_view name() const override {
      return "PostProcessingController";
    }
    void initialize() override;

    void prepare_filter_effect(unsigned int wHeight, unsigned int wWidth);
    void prepare_bloom_effect(unsigned int wHeight, unsigned int wWidth);
    void prepare_bloom_shaders();

    std::string get_active_filter() const;
    void set_active_filter(std::string filterShader);

    void draw() override;

    void begin_draw() override;
    void end_draw() override;

    void draw_bloom();
    void draw_filter();

    void terminate() override;
  };
}

#endif //POSTPROCESSINGCONTROLLER_HPP
