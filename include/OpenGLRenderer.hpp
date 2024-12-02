//
// Created by spaske on 12/1/24.
//

#ifndef RENDEREROPENGL_H
#define RENDEREROPENGL_H

namespace rg {
    class ShaderProgram;
    class Texture;
}

class SkyboxCube {
    friend class OpenGLRenderer;

public:
    static SkyboxCube create();

    uint32_t vao() const {
        return m_vao;
    }

    void destroy();

private:
    SkyboxCube() = default;

    uint32_t m_vao;

    explicit SkyboxCube(uint32_t skybox_vao) :
    m_vao(skybox_vao) {
    }
};

class OpenGLRenderer {
public:
    static OpenGLRenderer *instance();

    void end_frame();

    void begin_frame();

    void draw_skybox();

    void initialize();

    void terminate();

private:
    SkyboxCube m_skybox_cube;
};

#endif //RENDEREROPENGL_H
