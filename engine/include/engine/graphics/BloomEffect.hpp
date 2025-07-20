//
// Created by Jovana on 16.7.2025..
//

#ifndef BLOOMEFFECT_HPP
#define BLOOMEFFECT_HPP
#include <engine/resources/Shader.hpp>
#include <glm/glm.hpp>

namespace engine::graphics {

class BloomEffect {
public:
    BloomEffect();
    ~BloomEffect();

    void initialize(int width, int height, resources::Shader* blurShader, resources::Shader* finalShader);
    void resize(int width, int height);

    void begin();
    void end(const glm::vec2& viewportSize);

private:
     int width_{0}, height_{0};
     unsigned int  sceneFBO_{0};
     unsigned int  sceneColorTex_{0};
     unsigned int  brightTex_{0};
     unsigned int  depthRBO_{0};
     unsigned int  pingpongFBO_[2]{0,0};
     unsigned int  pingpongTex_[2]{0,0};

    resources::Shader* blurShader_ = nullptr;
    resources::Shader* finalShader_ = nullptr;

     unsigned int quadVAO_{0};
     unsigned int  quadVBO_{0};
     void drawQuad();
};

} // namespace engine::graphics
#endif //BLOOMEFFECT_HPP
