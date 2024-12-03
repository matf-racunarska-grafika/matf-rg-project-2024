//
// Created by spaske on 12/1/24.
//

#ifndef RENDEREROPENGL_H
#define RENDEREROPENGL_H

namespace rg {
    class Skybox;
    class ShaderProgram;
    class Texture;
}

class OpenGLRenderer {
public:
    static OpenGLRenderer *instance();

    void end_frame();

    void begin_frame();

    void draw_skybox();

    void initialize();

    void terminate();

private:
    rg::Skybox *m_skybox_cube;
};

#endif //RENDEREROPENGL_H
