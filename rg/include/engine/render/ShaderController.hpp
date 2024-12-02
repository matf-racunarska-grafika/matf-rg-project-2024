
//

#ifndef MATF_RG_PROJECT_SHADERCONTROLLER_HPP
#define MATF_RG_PROJECT_SHADERCONTROLLER_HPP

#include <memory>
#include <unordered_map>
#include <engine/controller/Controller.hpp>
#include <engine/render/Shader.hpp>

namespace rg {
    class ShaderController : public Controller {
    public:
        std::string_view name() const override {
            return "ShaderController";
        }

        static std::unique_ptr<ShaderController> create() {
            return std::make_unique<ShaderController>();
        }

        ShaderProgram *get(const ShaderName &shader_name) {
            return m_shaders.find(shader_name)->second.get();
        }

        std::string_view shader_language() const;

    private:
        void initialize() override;

        std::unordered_map<ShaderName, std::unique_ptr<ShaderProgram> > m_shaders;
        std::filesystem::path m_shaders_path;
    };

    using ShaderCompilationResult = std::variant<ShaderProgram, ShaderCompilationError, FileNotFoundError>;

    class ShaderCompiler {
    public:
        static ShaderCompilationResult compile_from_source(std::string shader_name, std::string shader_source);

        static ShaderCompilationResult compile_from_file(std::string shader_name, const std::filesystem::path &path);

    private:
        explicit ShaderCompiler(std::string shader_name, std::string shader_source)
        : m_shader_name(std::move(shader_name))
      , m_sources(std::move(shader_source)) {
        }

        ShaderProgram compile(const ShaderParsingResult &shader_sources);

        int compile(const std::string &shader_source, ShaderType type);

        ShaderParsingResult parse_source();

        std::string m_shader_name;
        std::string m_sources;
    };
} // namespace rg
#endif//MATF_RG_PROJECT_SHADERCONTROLLER_HPP
