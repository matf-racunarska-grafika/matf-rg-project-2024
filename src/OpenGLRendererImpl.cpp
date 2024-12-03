//
// Created by spaske on 12/1/24.
//
#include <AppStateController.hpp>
#include <glad/glad.h>

#include <OpenGLRenderer.hpp>
#include <engine/controller/ControllerManager.hpp>
#include <engine/platform/Platform.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/resources/Shader.hpp>
#include <engine/resources/ShaderController.hpp>
#include <engine/resources/Texture.hpp>

void OpenGLRenderer::initialize() {
    glEnable(GL_DEPTH_TEST);
    m_skybox_cube = rg::ControllerManager::get<rg::ResourcesController>()->skybox("skybox");
}

void OpenGLRenderer::begin_frame() {
    auto platform_controller = rg::ControllerManager::get<rg::PlatformController>();
    platform_controller->_platform_begin_frame();
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::end_frame() {
    auto platform_controller = rg::ControllerManager::get<rg::PlatformController>();
    platform_controller->_platform_end_frame();
}

void OpenGLRenderer::terminate() {
}

OpenGLRenderer *OpenGLRenderer::instance() {
    static std::unique_ptr<OpenGLRenderer> instance(new OpenGLRenderer);
    return instance.get();
}

void OpenGLRenderer::draw_skybox() {
    auto shader = rg::ControllerManager::get<rg::ShaderController>()->get("skybox");
    glDepthFunc(GL_LEQUAL);
    // change depth function so depth test passes when values are equal to depth buffer's content
    shader->use();
    auto state     = rg::ControllerManager::get<AppStateController>();
    glm::mat4 view = glm::mat4(glm::mat3(state->camera()->get_view_matrix()));
    // remove translation from the view matrix
    shader->set_mat4("view", view);
    shader->set_mat4("projection", state->projection());
    // skybox cube
    glBindVertexArray(m_skybox_cube->vao());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_cube->texture());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
