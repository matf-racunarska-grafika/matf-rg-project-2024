//
// Created by filipn on 10/10/25.
//

#ifndef POINTSHADOWCASTER_HPP
#define POINTSHADOWCASTER_HPP
#include <string>
#include <glm/vec3.hpp>

namespace engine::resources {
class Shader;
}

namespace engine::graphics {

class PointShadowCaster {
public:
    PointShadowCaster();

    void init(int shadowWidth = 1024, int shadowHeight = 1024, float nearPlane = 0.1f, float farPlane = 25.0f);

    void set_position(glm::vec3 &position);

    void render_pass();

    void set_shader(engine::resources::Shader *shader);

    void bind_to_shader(engine::resources::Shader *mainShader, const std::string &uniformNamePos,
                        const std::string &uniformNameFarPlane, const std::string &uniformNameShadowMap, int textureUnit);

private:
    void renderScene();

    void renderCube();

    uint32_t m_depthMapFBO;
    uint32_t m_depthCubeMap;
    int m_shadowWidth, m_shadowHeight;
    float m_nearPlane, m_farPlane;
    glm::vec3 m_position;
    engine::resources::Shader *m_shader;

    uint32_t m_cubeVAO;
    uint32_t m_cubeVBO;
};

}

#endif //POINTSHADOWCASTER_HPP
