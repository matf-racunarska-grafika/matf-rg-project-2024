
#ifndef MATF_RG_PROJECT_SHADER_HPP
#define MATF_RG_PROJECT_SHADER_HPP
#include <engine/controller/Controller.hpp>
#include <memory>
#include <string>
#include <utility>

namespace rg {
    using ShaderName = std::string;
    struct ShaderParsingResult;
    enum class ShaderType { Vertex, Fragment, Geometry };

    class ShaderProgram {
        friend class ShaderController;
        friend class ShaderCompiler;

    public:
        void use() const;
        void destroy() const;
        unsigned id() const {
            return m_shaderId;
        }

    private:
        explicit ShaderProgram(unsigned shaderId) : m_shaderId(shaderId) {
        }
        unsigned m_shaderId;
    };

    using ShaderCompilationResult = std::variant<ShaderProgram, ShaderCompilationError, FileNotFoundError>;

    class ShaderCompiler {
    public:
        static ShaderCompilationResult compile_from_source(std::string shader_name, std::string shader_source);
        static ShaderCompilationResult compile_from_file(std::string shader_name, const std::filesystem::path &path);

    private:
        explicit ShaderCompiler(std::string shader_name, std::string shader_source)
            : m_shader_name(std::move(shader_name)), m_sources(std::move(shader_source)) {
        }
        ShaderProgram compile(const ShaderParsingResult &shader_sources);
        int compile(const std::string &shader_source, ShaderType type);
        ShaderParsingResult parse_source();

        std::string m_shader_name;
        std::string m_sources;
    };
}// namespace rg

#endif//MATF_RG_PROJECT_SHADER_HPP
