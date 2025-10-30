#include <glad/glad.h>
#include <engine/resources/Shader.hpp>
#include <engine/graphics/OpenGL.hpp>

namespace engine::resources {

void Shader::prepare_for_use() {
    std::string uniform_name;
    uniform_name.reserve(32);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, graphics::OpenGL::getDefaultTexture());

    for (auto tex_type:{TextureType::Emissive,TextureType::Specular,TextureType::Diffuse})
        for (int i=0;i<m_num_textures[tex_type];i++) {
            set_int(std::string(Texture::uniform_name_convention(tex_type)) + std::to_string(i), i);
        }
}

int Shader::getLimitNumLights() const {
    return m_num_lights;
}
int Shader::getLimitNumTextures(TextureType type) const {
    return m_num_textures.at(type);
}
void Shader::use() const {
    glUseProgram(m_shader_id);
}

void Shader::destroy() const {
    glDeleteProgram(m_shader_id);
}

unsigned Shader::id() const {
    return m_shader_id;
}

void Shader::set_bool(const std::string &name, bool value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform1i, location, static_cast<int>(value));
}

void Shader::set_int(const std::string &name, int value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform1i, location, value);
}

void Shader::set_float(const std::string &name, float value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform1f, location, value);
}

void Shader::set_vec2(const std::string &name, const glm::vec2 &value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform2fv, location, 1, &value[0]);
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform3fv, location, 1, &value[0]);
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform4fv, location, 1, &value[0]);
}

void Shader::set_mat2(const std::string &name, const glm::mat2 &mat) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniformMatrix2fv, location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const std::string &name, const glm::mat3 &mat) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniformMatrix3fv, location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniformMatrix4fv, location, 1, GL_FALSE, &mat[0][0]);
}
void Shader::set_lights(const std::vector<engine::graphics::Light> &lights) const {
    std::string uniform_name;
    uniform_name.reserve(32);
    std::unordered_map<std::string_view, uint32_t> counts;

    for (size_t i = 0; i < lights.size(); i++) {
        const auto &light = lights[i];
        uniform_name = graphics::Light::uniform_name_convention(light.light_type());
        const auto count = (counts[uniform_name] += 1);

        uniform_name += "[" + std::to_string(count) + "].";
        graphics::LightData data = light.light_data();
        set_vec3(uniform_name + "position", data.position);
        set_vec3(uniform_name + "direction", data.direction);
        set_vec3(uniform_name + "ambient", data.ambient);
        set_vec3(uniform_name + "diffuse", data.diffuse);
        set_vec3(uniform_name + "specular", data.specular);
        set_float(uniform_name + "cutoff", data.cutoff);
        set_float(uniform_name + "outercutoff", data.outercutoff);
        set_float(uniform_name + "constant", data.constant);
        set_float(uniform_name + "linear", data.linear);
        set_float(uniform_name + "quadratic", data.quadratic);
    }
}


Shader::Shader(unsigned shader_id, std::string name, std::string source, std::filesystem::path source_path) :
                                                                                                            m_shader_id(shader_id)
                                                                                                          , m_name(std::move(name))
                                                                                                          , m_source(std::move(source))
                                                                                                          , m_source_path(std::move(source_path)) {

    m_num_textures[TextureType::Diffuse] = 1;
    m_num_textures[TextureType::Specular] = 1;
    m_num_textures[TextureType::Emissive] = 1;
    //prepare_for_use();

}

}
