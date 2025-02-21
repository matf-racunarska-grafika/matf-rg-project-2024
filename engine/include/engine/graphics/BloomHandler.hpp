//
// Created by viktor on 21.2.25..
//

#ifndef BLOOMHANDLER_HPP
#define BLOOMHANDLER_HPP

#include <engine/resources/Shader.hpp>

class BloomHandler {
private:
    BloomHandler() {}
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
    static void prepare_hdr_framebuffer();
    static void prepare_attachments();
    static void prepare_blur_framebuffers();
    static void prepare_shaders();
    static void fill_pingpong_buffers();
    static void render_quad();
};



#endif //BLOOMHANDLER_HPP
