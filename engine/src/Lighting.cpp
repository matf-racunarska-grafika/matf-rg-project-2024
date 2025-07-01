#include <engine/graphics/Lighting.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

namespace engine::graphics::lighting {

static constexpr float NEAR_PLANE = 1.0f;
static constexpr float FAR_PLANE = 25.0f;

LightingSystem::LightingSystem(unsigned shadowWidth,
                               unsigned shadowHeight)
: m_shadowW(shadowWidth)
, m_shadowH(shadowHeight)
, m_depthFBO(0)
, m_depthCubemap(0)
, m_depthShader(nullptr)
, m_lightingShader(nullptr)
, m_hasPointLight(false) {}

void LightingSystem::initialize() {
    auto *res = engine::core::Controller::get<engine::resources::ResourcesController>();
    m_depthShader = res->shader("point_shadows_depth");
    m_lightingShader = res->shader("lighting");

    glGenFramebuffers(1, &m_depthFBO);
    glGenTextures(1, &m_depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
    for (unsigned i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_DEPTH_COMPONENT,
                     m_shadowW, m_shadowH,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { spdlog::error("LightingSystem: depth-cubemap FBO nije complete!"); }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightingSystem::setDirectionalLight(const DirectionalLight &dl) { m_dirLight = dl; }

void LightingSystem::setPointLight(const PointLight &pl) {
    m_pointLight = pl;
    m_hasPointLight = true;
}

void LightingSystem::clearPointLight() { m_hasPointLight = false; }

void LightingSystem::setMaterial(const Material &mat) { m_material = mat; }

void LightingSystem::setCameraPosition(const glm::vec3 &camPos) { m_cameraPos = camPos; }

void LightingSystem::setLightIntensity(float intensity) { m_lightIntensity = intensity; }

void LightingSystem::beginDepthPass() {
    glDisable(GL_MULTISAMPLE);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthFBO);
    glViewport(0, 0, m_shadowW, m_shadowH);
    glClear(GL_DEPTH_BUFFER_BIT);
    updateShadowMatrices();
}

void LightingSystem::renderDepthPass(const std::function<void(const Shader *)> &drawScene) {
    m_depthShader->use();
    for (int i = 0; i < 6; ++i) { m_depthShader->set_mat4("shadowMatrices[" + std::to_string(i) + "]", m_shadowMats[i]); }
    if (m_hasPointLight) {
        m_depthShader->set_vec3("lightPos", m_pointLight.position);
        m_depthShader->set_float("far_plane", FAR_PLANE);
    }
    drawScene(m_depthShader);
}

void LightingSystem::endDepthPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightingSystem::renderLightingPass(const std::function<void(const Shader *)> &drawScene) {
    m_lightingShader->use();
    uploadUniforms(*m_lightingShader);

    if (m_hasPointLight) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
        m_lightingShader->set_int("shadowMap", 2);
    }

    drawScene(m_lightingShader);
}

void LightingSystem::renderLightBulb(const glm::vec3 &pos, float scale) {
    auto *graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto *basicShader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("unlit_white");
    auto *cubeModel = engine::core::Controller::get<engine::resources::ResourcesController>()->model("light_bulb");

    basicShader->use();
    basicShader->set_mat4("projection", graphics->projection_matrix());
    basicShader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), pos);
    modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    modelMat = glm::scale(modelMat, glm::vec3(scale));
    basicShader->set_mat4("model", modelMat);

    cubeModel->draw(basicShader);
}

void LightingSystem::updateShadowMatrices() {
    float aspect = float(m_shadowW) / float(m_shadowH);
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), aspect, NEAR_PLANE, FAR_PLANE);
    if (!m_hasPointLight) return;

    glm::vec3 pos = m_pointLight.position;
    m_shadowMats[0] = proj * glm::lookAt(pos, pos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));
    m_shadowMats[1] = proj * glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
    m_shadowMats[2] = proj * glm::lookAt(pos, pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    m_shadowMats[3] = proj * glm::lookAt(pos, pos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
    m_shadowMats[4] = proj * glm::lookAt(pos, pos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
    m_shadowMats[5] = proj * glm::lookAt(pos, pos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
}

void LightingSystem::uploadUniforms(Shader &shader) {
    shader.set_vec3("gDirectionalLight.Base.Color", m_dirLight.base.color);
    shader.set_float("gDirectionalLight.Base.AmbientIntensity", m_dirLight.base.ambientIntensity);
    shader.set_float("gDirectionalLight.Base.DiffuseIntensity", m_dirLight.base.diffuseIntensity);
    shader.set_vec3("gDirectionalLight.Direction", m_dirLight.direction);

    shader.set_int("gNumPointLights", m_hasPointLight ? 1 : 0);
    if (m_hasPointLight) {
        shader.set_vec3("gPointLights[0].Base.Color", m_pointLight.base.color);
        shader.set_float("gPointLights[0].Base.AmbientIntensity", m_pointLight.base.ambientIntensity);
        shader.set_float("gPointLights[0].Base.DiffuseIntensity", m_pointLight.base.diffuseIntensity);
        shader.set_vec3("gPointLights[0].LocalPos", m_pointLight.position);
        shader.set_float("gPointLights[0].Atten.Constant", m_pointLight.atten.constant);
        shader.set_float("gPointLights[0].Atten.Linear", m_pointLight.atten.linear);
        shader.set_float("gPointLights[0].Atten.Exp", m_pointLight.atten.exp);
    }

    shader.set_vec3("gMaterial.AmbientColor", m_material.ambientColor);
    shader.set_vec3("gMaterial.DiffuseColor", m_material.diffuseColor);
    shader.set_vec3("gMaterial.SpecularColor", m_material.specularColor);

    shader.set_vec3("gCameraLocalPos", m_cameraPos);
    shader.set_float("uLightIntensity", m_lightIntensity);
    shader.set_bool("shadows", m_hasPointLight);

    if (m_hasPointLight) {
        shader.set_vec3("lightPos", m_pointLight.position);
        shader.set_float("far_plane", FAR_PLANE);
    }
}

}// namespace engine::graphics::lighting
