//
// Created by filipn on 10/10/25.
//

#include <engine/graphics/PointShadowCaster.hpp>

#include <glad/glad.h>

#include <GL/gl.h>
#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace engine::platform {
class PlatformController;
}

namespace engine::graphics {
PointShadowCaster::PointShadowCaster()
: m_depthMapFBO(0)
, m_depthCubeMap(0)
, m_shadowWidth(0)
, m_shadowHeight(0)
, m_nearPlane(0.1f)
, m_farPlane(25.0f)
, m_position(0.0f)
, m_shader(nullptr) {}

void PointShadowCaster::init(int shadowWidth, int shadowHeight, float nearPlane, float farPlane) {
    m_shadowWidth = shadowWidth;
    m_shadowHeight = shadowHeight;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;

    glGenFramebuffers(1, &m_depthMapFBO);
    glGenTextures(1, &m_depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);

    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { spdlog::error("PointShadowCaster: Framebuffer is not complete"); }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointShadowCaster::set_position(glm::vec3 &position) { m_position = position; }

void PointShadowCaster::render_pass() {
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float) m_shadowWidth / (float) m_shadowHeight, m_nearPlane, m_farPlane);
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(m_position, m_position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(m_position, m_position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(m_position, m_position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(m_position, m_position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(m_position, m_position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(m_position, m_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    glViewport(0, 0, m_shadowWidth, m_shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_shader->use();
    for (unsigned int i = 0; i < 6; ++i) { m_shader->set_mat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]); }
    m_shader->set_float("far_plane", m_farPlane);
    m_shader->set_vec3("lightPos", m_position);

    if (m_renderSceneCallback) { m_renderSceneCallback(m_shader); } else { spdlog::warn("PointShadowCaster: No render scene callback set."); }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    glViewport(0, 0, platform->window()->width(), platform->window()->height());
}

void PointShadowCaster::set_shader(resources::Shader *shader) { m_shader = shader; }

void PointShadowCaster::bind_to_shader(resources::Shader *mainShader, const std::string &uniformNamePos,
                                       const std::string &uniformNameFarPlane, const std::string &uniformNameShadowMap, int textureUnit) {
    mainShader->use();
    mainShader->set_vec3(uniformNamePos, m_position);
    mainShader->set_float(uniformNameFarPlane, m_farPlane);
    mainShader->set_int(uniformNameShadowMap, textureUnit);

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
}

void PointShadowCaster::set_render_scene_callback(std::function<void(resources::Shader *)> callback) { m_renderSceneCallback = std::move(callback); }

}
