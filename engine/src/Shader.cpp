//
// Created by spaske on 12/2/24.
//
#include <glad/glad.h>
#include <engine/render/Shader.hpp>

namespace rg {

    void ShaderProgram::use() const {
        glUseProgram(m_shaderId);
    }

    void ShaderProgram::destroy() const {
        glDeleteProgram(m_shaderId);
    }

    unsigned ShaderProgram::id() const {
        return m_shaderId;
    }

    void ShaderProgram::set_bool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(m_shaderId, name.c_str()), static_cast<int>(value));
    }

    void ShaderProgram::set_int(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(m_shaderId, name.c_str()), value);
    }

    void ShaderProgram::set_float(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(m_shaderId, name.c_str()), value);
    }

    void ShaderProgram::set_vec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }

    void ShaderProgram::set_vec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(m_shaderId, name.c_str()), x, y);
    }

    void ShaderProgram::set_vec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }

    void ShaderProgram::set_vec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(m_shaderId, name.c_str()), x, y, z);
    }

    void ShaderProgram::set_vec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }

    void ShaderProgram::set_vec4(const std::string &name, float x, float y, float z, float w) {
        glUniform4f(glGetUniformLocation(m_shaderId, name.c_str()), x, y, z, w);
    }

    void ShaderProgram::set_mat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderProgram::set_mat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderProgram::set_mat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    ShaderProgram::ShaderProgram(unsigned shaderId) :
    m_shaderId(shaderId) {
    }

}
