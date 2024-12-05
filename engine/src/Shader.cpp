//
// Created by spaske on 12/2/24.
//
#include <glad/glad.h>
#include <engine/resources/Shader.hpp>

namespace rg {

    void Shader::use() const {
        glUseProgram(m_shaderId);
    }

    void Shader::destroy() const {
        glDeleteProgram(m_shaderId);
    }

    unsigned Shader::id() const {
        return m_shaderId;
    }

    void Shader::set_bool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(m_shaderId, name.c_str()), static_cast<int>(value));
    }

    void Shader::set_int(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(m_shaderId, name.c_str()), value);
    }

    void Shader::set_float(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(m_shaderId, name.c_str()), value);
    }

    void Shader::set_vec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(m_shaderId, name.c_str()), x, y);
    }

    void Shader::set_vec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(m_shaderId, name.c_str()), x, y, z);
    }

    void Shader::set_vec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec4(const std::string &name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(m_shaderId, name.c_str()), x, y, z, w);
    }

    void Shader::set_mat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    Shader::Shader(unsigned shaderId) :
    m_shaderId(shaderId) {
    }

}
