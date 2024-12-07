#ifndef MATF_RG_PROJECT_SHADER_HPP
#define MATF_RG_PROJECT_SHADER_HPP
#include <rg/util/Utils.hpp>
#include <string>
#include <glm/glm.hpp>
#include <utility>

namespace rg {
    using ShaderName = std::string;

    class Shader {
        friend class ShaderController;
        friend class ShaderCompiler;

    public:
        void use() const;

        void destroy() const;

        unsigned id() const;

        void set_bool(const std::string &name, bool value) const;

        void set_int(const std::string &name, int value) const;

        void set_float(const std::string &name, float value) const;

        void set_vec2(const std::string &name, const glm::vec2 &value) const;

        void set_vec2(const std::string &name, float x, float y) const;

        void set_vec3(const std::string &name, const glm::vec3 &value) const;

        void set_vec3(const std::string &name, float x, float y, float z) const;

        void set_vec4(const std::string &name, const glm::vec4 &value) const;

        void set_vec4(const std::string &name, float x, float y, float z, float w) const;

        void set_mat2(const std::string &name, const glm::mat2 &mat) const;

        void set_mat3(const std::string &name, const glm::mat3 &mat) const;

        void set_mat4(const std::string &name, const glm::mat4 &mat) const;

        const std::string &name() const;

        const std::string &source() const;

        const std::filesystem::path &source_path() const;

    private:
        Shader(unsigned shader_id, std::string name, std::string source,
               std::filesystem::path source_path = "");

        unsigned m_shaderId;
        std::string m_name;
        std::string m_source;
        std::filesystem::path m_source_path;
    };
} // namespace rg

#endif//MATF_RG_PROJECT_SHADER_HPP
