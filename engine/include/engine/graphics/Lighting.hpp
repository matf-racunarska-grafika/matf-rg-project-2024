#pragma once

#include <glm/glm.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::graphics::lighting {

using Shader = engine::resources::Shader;

// ----- Tipovi svetla i materijala -----
struct BaseLight {
    glm::vec3 color;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight {
    BaseLight base;
    glm::vec3 direction;
};

struct Attenuation {
    float constant;
    float linear;
    float exp;
};

struct PointLight {
    BaseLight base;
    glm::vec3 position;
    Attenuation atten;
};

struct Material {
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
};

// ----- Glavna klasa za osvetljenje (samo jedno point svetlo) -----
class LightingSystem {
public:
    LightingSystem(unsigned shadowWidth,
                   unsigned shadowHeight);

    void initialize();

    // Directional
    void setDirectionalLight(const DirectionalLight &dl);

    // Point light (0 ili 1)
    void setPointLight(const PointLight &pl);

    void clearPointLight();

    // Materijal + kamera + intenzitet
    void setMaterial(const Material &mat);

    void setCameraPosition(const glm::vec3 &camPos);

    void setLightIntensity(float intensity);

    // Shadow‐map pass
    void beginDepthPass();

    void renderDepthPass(const std::function<void(const Shader *)> &drawScene);

    void endDepthPass();

    // Main lighting pass
    void renderLightingPass(const std::function<void(const Shader *)> &drawScene);

    // Crtanje modela izvora point svetla
    void renderLightBulb(const glm::vec3 &pos, float scale);

private:
    unsigned m_shadowW, m_shadowH;

    unsigned m_depthFBO, m_depthCubemap;
    Shader *m_depthShader;
    Shader *m_lightingShader;

    DirectionalLight m_dirLight{};
    bool m_hasPointLight = false;
    PointLight m_pointLight{};
    Material m_material{};
    glm::vec3 m_cameraPos{};
    float m_lightIntensity{1.0f};

    glm::mat4 m_shadowMats[6];

    void updateShadowMatrices();

    void uploadUniforms(Shader &shader);
};

}// namespace engine::graphics::lighting
