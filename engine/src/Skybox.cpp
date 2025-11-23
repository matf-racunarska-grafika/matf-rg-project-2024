// clang-format off
#include <glad/glad.h>
// clang-format on
#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/Skybox.hpp>

namespace engine::resources {
void Skybox::destroy() {
    CHECKED_GL_CALL(glDeleteVertexArrays, 1, &m_vao);
}
}// namespace engine::resources