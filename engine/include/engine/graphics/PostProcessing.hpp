//
// Created by matfrg on 10/19/25.
//

#ifndef POSTPROCESSING_HPP
#define POSTPROCESSING_HPP
#include <engine/resources/Shader.hpp>

namespace engine::graphics{

class GraphicsController;

class PostProcessing {
    friend class GraphicsController;
public:
    void init_hdr(int width,int height);
    void resize_hdr(int width,int height);
    void bind_hdr_framebuffer();
    void unbind_hdr_framebuffer();

    unsigned int get_scene_texture();//za tonemapping
    unsigned int get_bright_texture(); //za bloom (ipak pristupamo direktno)
    unsigned int get_pingpong_texture() {
        return m_pingpong_color_buffers[1];
    }

    void apply_gaussian_blur(engine::resources::Shader* blur_shader);

    void render_screen_quad();
private:
    void init_bloom(int width,int height);
    void resize_bloom(int width, int height);

    unsigned int m_hdr_fbo;
    unsigned int m_color_buffers[2];
    unsigned int m_rbo_depth;

    unsigned int m_quad_vao=0;
    unsigned int m_quad_vbo=0;
    float m_exposure=0.4f;

    unsigned int m_pingpong_fbo[2];
    unsigned int m_pingpong_color_buffers[2];
    int m_blur_itterartions = 10;
    float m_bloom_strength = 1.0f;
    bool m_bloom_enabled = true;
};


}

#endif //POSTPROCESSING_HPP
