//
// Created by matija on 10/29/25.
//

#ifndef MATF_RG_PROJECT_MYFRAMEBUFFER_HPP
#define MATF_RG_PROJECT_MYFRAMEBUFFER_HPP
#include <string>
#include <unordered_map>
#include <engine/graphics/OpenGL.hpp>

namespace engine::resources {



class MyFrameBuffer {
public:
    MyFrameBuffer();
    ~MyFrameBuffer();

    void init(int width, int height, bool use_depth=true);
    void free();

    void add_color_attachment(graphics::FrameTextureType type, bool linear,const std::string& name);

    void bind() const;
    void clear();
    static void unbind();
    void bind_texture(std::string name, unsigned int slot);


    unsigned int framebuffer_id() const { return m_fbo; }
    unsigned int depth_buffer() const { return m_depth_buffer; }
    unsigned int texture(const std::string& name)  { return m_color_textures[name]; }
    size_t texture_count() const { return m_color_textures.size(); }

    bool is_complete() const {
        return (engine::graphics::OpenGL::is_framebuffer_complete());
    }
private:
    unsigned int m_fbo = 0;
    unsigned int m_depth_buffer = 0;
    std::unordered_map<std::string,unsigned int> m_color_textures;

    int m_width = 0;
    int m_height = 0;
};

}


#endif //MATF_RG_PROJECT_MYFRAMEBUFFER_HPP