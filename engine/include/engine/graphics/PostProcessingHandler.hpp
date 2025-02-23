#ifndef POSTPROCESSINGHANDLER_HPP
#define POSTPROCESSINGHANDLER_HPP

#include <engine/resources/Shader.hpp>

class PostProcessingHandler {
private:
    PostProcessingHandler() = default;
    ~PostProcessingHandler() {}
    PostProcessingHandler(const PostProcessingHandler&) = delete;
    PostProcessingHandler& operator=(const PostProcessingHandler&) = delete;

    static unsigned int hdrFBO;
    static unsigned int colorBuffers[2];
    static unsigned int attachments[2];
    static unsigned int pingpongFBO[2];
    static unsigned int pingpongBuffer[2];
    static unsigned int screenFBO;
    static unsigned int screenTexture;

    static engine::resources::Shader* bloom, *blur, *bloom_final;
    static engine::resources::Shader* negative, *grayscale, *deepfried, *none;
public:
    static void initialise();
    static void draw();
    static void render_quad();
    static void prepare_post_processing_framebuffer();
    static void prepare_hdr_framebuffer();
    static void prepare_attachments();
    static void prepare_blur_framebuffers();
    static void prepare_shaders();
    static void compose();
    static void bind_hdr_framebuffer();
    static void unbind_framebuffer();
    static void prepare_screen_framebuffer();
    static void apply_filters();
};

#endif //POSTPROCESSINGHANDLER_HPP
