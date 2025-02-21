//
// Created by viktor on 21.2.25..
//

#ifndef BLOOMHANDLER_HPP
#define BLOOMHANDLER_HPP

#include <engine/resources/Shader.hpp>

class BloomHandler {
private:
    BloomHandler() = default;
    ~BloomHandler() {}
    BloomHandler(const BloomHandler&) = delete;
    BloomHandler& operator=(const BloomHandler&) = delete;

    static unsigned int hdrFBO;
    static unsigned int colorBuffers[2];
    static unsigned int attachments[2];
    static unsigned int pingpongFBO[2];
    static unsigned int pingpongBuffer[2];

    static engine::resources::Shader* bloom, *blur, *bloom_final;
public:
    static void initialise();
    static void draw();
    static void render_quad();
    static void prepare_hdr_framebuffer();
    static void prepare_attachments();
    static void prepare_blur_framebuffers();
    static void prepare_shaders();
    static void compose();
    static void bind_framebuffer();
    static void unbind_framebuffer();
};

#endif //BLOOMHANDLER_HPP
