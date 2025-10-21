//
// Created by matfrg on 10/19/25.
//
#include "engine/graphics/PostProcessing.hpp"

#include "spdlog/spdlog.h"

#include <glad/glad.h>


namespace engine::graphics {

void PostProcessing::init_hdr(int width, int height) {
    glGenFramebuffers(1,&m_hdr_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,m_hdr_fbo);

    glGenTextures(2,m_color_buffers);
    for(unsigned int i=0;i<2;i++) {
        glBindTexture(GL_TEXTURE_2D,m_color_buffers[i]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,width,height,
            0,GL_RGBA,GL_FLOAT,nullptr);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0+i,
            GL_TEXTURE_2D,m_color_buffers[i],0);
    }
    glGenRenderbuffers(1,&m_rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER,m_rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,m_rbo_depth);

    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2,attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) {
        spdlog::error("HDR Framebuffer not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void PostProcessing::resize_hdr(int width, int height) {
    glDeleteFramebuffers(1,&m_hdr_fbo);
    glDeleteRenderbuffers(1,&m_rbo_depth);
    glDeleteTextures(2,m_color_buffers);

    init_hdr(width,height);
}


void PostProcessing::bind_hdr_framebuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER,m_hdr_fbo);
}
void PostProcessing::unbind_hdr_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}
unsigned int PostProcessing::get_scene_texture() {
    return m_color_buffers[0];
}
unsigned int PostProcessing::get_bright_texture() {
    return m_color_buffers[1];
}

void PostProcessing::render_screen_quad() {
    if(m_quad_vao==0) {
        float quadVerticies[] = {
            //pozicija   //tekstura
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };
        glGenVertexArrays(1,&m_quad_vao);
        glGenBuffers(1,&m_quad_vbo);
        glBindVertexArray(m_quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER,m_quad_vbo);
        glBufferData(GL_ARRAY_BUFFER,sizeof(quadVerticies),
            quadVerticies,GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,
            4*sizeof(float),(void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,
            4*sizeof(float),(void*)(2*sizeof(float)));
    }

    glBindVertexArray(m_quad_vao);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
}

void PostProcessing::init_bloom(int width, int height) {
    glGenFramebuffers(2,m_pingpong_fbo);
    glGenTextures(2,m_pingpong_color_buffers);
    for(unsigned int i = 0 ; i<2;i++) {
        glBindFramebuffer(GL_FRAMEBUFFER,m_pingpong_fbo[i]);
        glBindTexture(GL_TEXTURE_2D,m_pingpong_color_buffers[i]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,width,height,
            0,GL_RGBA,GL_FLOAT,nullptr);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,m_pingpong_color_buffers[i],0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) {
            spdlog::error("Pingpong framebuffer {} is not complete!",i);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void PostProcessing::resize_bloom(int width, int height) {
    for(unsigned int i=0;i<2;i++) {
        glBindTexture(GL_TEXTURE_2D,m_pingpong_color_buffers[i]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,width,height,
            0,GL_RGBA,GL_FLOAT,nullptr);
    }
}

void PostProcessing::apply_gaussian_blur(engine::resources::Shader *blur_shader) {
    bool horizontal = true;
    bool first_itteration = true;
    for(int i=0; i<m_blur_itterartions;++i) {
        glBindFramebuffer(GL_FRAMEBUFFER,m_pingpong_fbo[horizontal]);
        blur_shader->use();
        blur_shader->set_bool("horizontal",horizontal);

        glActiveTexture(GL_TEXTURE0);
        unsigned int texture = first_itteration ? m_color_buffers[1] : m_pingpong_color_buffers[!horizontal];
        glBindTexture(GL_TEXTURE_2D,texture);
        blur_shader->set_int("image",0);

        render_screen_quad();
        horizontal = !horizontal;
        if(first_itteration) {
            first_itteration=false;
        }

    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}



}// namespace engine::graphics