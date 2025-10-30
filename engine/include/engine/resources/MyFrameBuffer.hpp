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

    void addColorAttachment(graphics::FrameTextureType type, bool linear,const std::string& name);

    void bind() const;
    void clear();
    static void unbind();
    void bindTexture(std::string name, unsigned int slot);


    unsigned int getFrameBuff() const { return fbo; }
    unsigned int getDepthBuffer() const { return depthBuffer; }
    unsigned int getTexture(const std::string& name)  { return colorTextures[name]; }
    size_t getTextureCount() const { return colorTextures.size(); }

private:
    unsigned int fbo = 0;
    unsigned int depthBuffer = 0;
    std::unordered_map<std::string,unsigned int> colorTextures;

    int width = 0;
    int height = 0;
};

}


#endif //MATF_RG_PROJECT_MYFRAMEBUFFER_HPP