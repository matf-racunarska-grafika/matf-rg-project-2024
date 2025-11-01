#include <glad/glad.h>
#include <engine/graphics/SpriteController.hpp>
#include <engine/resources/Texture.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::graphics {

void SpriteController::initialize() {
    if (m_vao != 0) return;
    float vertices[] = {
            // pos                 // uv
            0.0f,   0.0f,  0.905f,   0.0f, 0.0f,
            0.846f, 0.0f,  0.905f,   1.0f, 0.0f,
            0.0f,   0.0f,  0.0f,     0.0f, 1.0f,
            0.0f,   0.0f,  0.0f,     0.0f, 1.0f,
            0.846f, 0.0f,  0.905f,   1.0f, 0.0f,
            0.846f, 0.0f,  0.0f,     1.0f, 1.0f
    };
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}

void SpriteController::terminate() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    m_vao = m_vbo = 0;
}

void SpriteController::draw_textured_quad(resources::Texture *texture,
                                          resources::Shader *shader,
                                          const glm::mat4 &model,
                                          const glm::mat4 &view,
                                          const glm::mat4 &projection) {
    if (m_vao == 0) initialize();
    shader->use();
    shader->set_mat4("model", model);
    shader->set_mat4("view", view);
    shader->set_mat4("projection", projection);

    // Uvek nacrtaj sprite iznad scene
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture ? texture->id() : 0);
    shader->set_int("uTex", 0);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

}


