//
// Created by matfrg on 10/19/25.
//

#ifndef POSTPROCESSING_HPP
#define POSTPROCESSING_HPP

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
    unsigned int get_bright_texture(); //za bloom

    void render_screen_quad();
private:
    unsigned int m_hdr_fbo;
    unsigned int m_color_buffers[2];
    unsigned int m_rbo_depth;

    unsigned int m_quad_vao=0;
    unsigned int m_quad_vbo=0;
    float m_exposure=1;
};


}

#endif //POSTPROCESSING_HPP
