//
// Created by filipn on 10/10/25.
//

#ifndef POINTSHADOWCASTER_HPP
#define POINTSHADOWCASTER_HPP
#include <functional>
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

    void set_shader(resources::Shader *shader);

    void bind_to_shader(resources::Shader *mainShader, const std::string &uniformNamePos,
                        const std::string &uniformNameFarPlane, const std::string &uniformNameShadowMap, int textureUnit);

    void set_render_scene_callback(std::function<void(resources::Shader *)> callback);

private:
    uint32_t m_depthMapFBO;
    uint32_t m_depthCubeMap;
    int m_shadowWidth, m_shadowHeight;
    float m_nearPlane, m_farPlane;
    glm::vec3 m_position;
    engine::resources::Shader *m_shader;
    std::function<void(resources::Shader *)> m_renderSceneCallback;
};

}

#endif //POINTSHADOWCASTER_HPP
