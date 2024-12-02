//
// Created by spaske on 12/1/24.
//
#include <OpenGLRenderer.hpp>
#include <glad/glad.h>

#include <engine/controller/ControllerManager.hpp>
#include <engine/platform/Platform.hpp>

void OpenGLRenderer::initialize() {
    glEnable(GL_DEPTH_TEST);
}

void OpenGLRenderer::begin_frame() {
    auto platform_controller = rg::ControllerManager::get<rg::PlatformController>();
    platform_controller->_platform_begin_frame();
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

OpenGLRenderer *OpenGLRenderer::instance() {
    static std::unique_ptr<OpenGLRenderer> instance = std::make_unique<OpenGLRenderer>();
    return instance.get();
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
