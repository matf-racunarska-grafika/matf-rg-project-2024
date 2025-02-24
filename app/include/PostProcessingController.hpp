#ifndef POSTPROCESSINGCONTROLLER_HPP
#define POSTPROCESSINGCONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace app {
  class PostProcessingController : public engine::core::Controller {
    unsigned int hdrFBO = 0;
    unsigned int colorBuffers[2] = { 0, 0 };
    unsigned int attachments[2] = { 0, 0 };
    unsigned int pingpongFBO[2] = { 0, 0 };
    unsigned int pingpongBuffer[2] = { 0, 0 };
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;
    unsigned int screenFBO = 0;
    unsigned int screenTexture = 0;

  public:
    void initialize() override;

    void prepare_quad();
    void prepare_filter_effect(unsigned int wHeight, unsigned int wWidth);
    void prepare_bloom_effect(unsigned int wHeight, unsigned int wWidth);
    void prepare_bloom_shaders();

    void draw() override;

    void begin_draw() override;
    void end_draw() override;

    void render_quad();
    void draw_filters();
    void draw_health_bar();
    void draw_bloom();

    void terminate() override;

  };
}

#endif //POSTPROCESSINGCONTROLLER_HPP
