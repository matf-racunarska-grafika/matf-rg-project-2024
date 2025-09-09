//
// Created by boce on 7/26/25.
//

#include "../../engine/libs/glad/include/glad/glad.h"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"

#include <MainController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void MainController::initialize() {
    spdlog::info("Main controller successfully initialized");
    create_plane();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).state() == engine::platform::Key::State::Pressed) { return false; }
    return true;
}

void MainController::begin_draw() { engine::graphics::OpenGL::enable_depth_testing(); }

void MainController::draw() { draw_plane(); }

void MainController::end_draw() { engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers(); }


void MainController::create_plane() {
    float vertices[] = {
            -5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f,
            -5.0f, -0.5f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            5.0f, -0.5f, 5.0f, 0.0f, 1.0f, 0.0f, 5.0f, 0.0f,

            5.0f, -0.5f, 5.0f, 0.0f, 1.0f, 0.0f, 5.0f, 0.0f,
            5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 5.0f, 5.0f,
            -5.0f, -0.5f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f
    };

    CHECKED_GL_CALL(glGenBuffers, 1, &vbo_plane);
    CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo_plane);
    CHECKED_GL_CALL(glGenVertexArrays, 1, &vao_plane);
    CHECKED_GL_CALL(glBindVertexArray, vao_plane);
    CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    CHECKED_GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    CHECKED_GL_CALL(glEnableVertexAttribArray, 0);

    CHECKED_GL_CALL(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    CHECKED_GL_CALL(glEnableVertexAttribArray, 1);

    CHECKED_GL_CALL(glVertexAttribPointer, 2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    CHECKED_GL_CALL(glEnableVertexAttribArray, 2);

    CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}

void MainController::draw_plane() {}


}// app