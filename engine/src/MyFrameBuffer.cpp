//
// Created by matija on 10/29/25.
//

#include "../include/engine/resources/MyFrameBuffer.hpp"

using namespace engine::resources;

MyFrameBuffer::MyFrameBuffer() {}

MyFrameBuffer::~MyFrameBuffer() {
    free();
}

void MyFrameBuffer::init(int width, int height, bool use_depth) {
    this->width = width;
    this->height = height;

    fbo = engine::graphics::OpenGL::genFrameBuffer();
    if (use_depth)
        depthBuffer = graphics::OpenGL::addRenderBuffer(fbo, width, height);
}

void MyFrameBuffer::addColorAttachment(graphics::FrameTextureType type, bool linear, const std::string &name) {
    unsigned int tex = graphics::OpenGL::addFrameTexture(fbo,  getTextureCount(), type, width, height, linear);
    colorTextures[name] = tex;
    graphics::OpenGL::setAttachmentCount(fbo, getTextureCount());
}

void MyFrameBuffer::bind() const {
    graphics::OpenGL::bindFrameBuffer(fbo);
}

void MyFrameBuffer::clear() {
    graphics::OpenGL::clear_buffers();
}

void MyFrameBuffer::unbind() {
    graphics::OpenGL::bindFrameBuffer(0);
}

void MyFrameBuffer::bindTexture(std::string name, unsigned int slot) {
    graphics::OpenGL::BindTexture(colorTextures[name], slot);
}


void MyFrameBuffer::free() {
    if (depthBuffer) {
        graphics::OpenGL::deleteRenderBuffer(depthBuffer);
        depthBuffer = 0;
    }

    if (!colorTextures.empty()) {
        for (auto pair: colorTextures) {
            graphics::OpenGL::deleteTexture(pair.second);
        }
        colorTextures.clear();
    }

    if (fbo) {
        graphics::OpenGL::deleteFrameBuffer(fbo);
        fbo = 0;
    }
}
