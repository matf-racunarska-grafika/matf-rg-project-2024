//
// Created by spaske on 12/1/24.
//
#include <AppStateController.hpp>
#include <glad/glad.h>

#include <OpenGLRenderer.hpp>
#include <engine/controller/ControllerManager.hpp>
#include <engine/platform/Platform.hpp>
#include <engine/render/ResourcesController.hpp>
#include <engine/render/Shader.hpp>
#include <engine/render/ShaderController.hpp>
#include <engine/render/Texture.hpp>

SkyboxCube SkyboxCube::create() {
    float skyboxVertices[] = {
            // positions
            -1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            -1.0f,

            -1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            1.0f,

            1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            -1.0f,

            -1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            1.0f,

            -1.0f,
            1.0f,
            -1.0f,
            1.0f,
            1.0f,
            -1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            -1.0f,
            1.0f,
            1.0f,
            -1.0f,
            1.0f,
            -1.0f,

            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            1.0f,
            -1.0f,
            1.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    return SkyboxCube(skyboxVAO);
}

void SkyboxCube::destroy() {
    glDeleteVertexArrays(1, &m_vao);
}

void OpenGLRenderer::draw_skybox() {
    auto shader  = rg::ControllerManager::get<rg::ShaderController>()->get("skybox");
    auto texture = rg::ControllerManager::get<rg::ResourcesController>()->skybox("skybox");
    glDepthFunc(GL_LEQUAL);
    // change depth function so depth test passes when values are equal to depth buffer's content
    shader->use();
    auto state     = rg::ControllerManager::get<AppStateController>();
    glm::mat4 view = glm::mat4(glm::mat3(state->camera()->get_view_matrix()));
    // remove translation from the view matrix
    shader->set_mat4("view", view);
    shader->set_mat4("projection", state->projection());
    // skybox cube
    glBindVertexArray(m_skybox_cube.vao());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void OpenGLRenderer::initialize() {
    glEnable(GL_DEPTH_TEST);
    m_skybox_cube = SkyboxCube::create();
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
