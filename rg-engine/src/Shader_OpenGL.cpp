
//
#include "engine/renderer/ShaderController.hpp"
#include <engine/controller/ControllerManager.hpp>
#include <engine/platform/Platform.hpp>
#include <engine/renderer/Shader.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <utility>

namespace rg {

    int to_opengl_type(ShaderType type);
    static std::string_view to_string(ShaderType type);

    struct ShaderParsingResult {
        std::string vertex_shader;
        std::string fragment_shader;
        std::string geometry_shader;
    };

    void ShaderProgram::use() const {
        glUseProgram(m_shaderId);
    }

    void ShaderProgram::destroy() const {
        glDeleteProgram(m_shaderId);
    }

    ShaderCompilationResult ShaderCompiler::compile_from_source(std::string shader_name, std::string shader_source) {
        try {
            spdlog::info("Compiling: {}", shader_name);
            ShaderCompiler compiler(std::move(shader_name), std::move(shader_source));
            ShaderParsingResult parsing_result = compiler.parse_source();
            ShaderProgram shader_program = compiler.compile(parsing_result);
            return shader_program;
        } catch (const ShaderCompilationError &e) { return e; }
    }

    bool opengl_compilation_failed(int shader_id) {
        int success;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        return !success;
    };

    std::string opengl_get_compilation_error_message(int shader_id) {
        char infoLog[512];
        glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
        return infoLog;
    }

    ShaderProgram ShaderCompiler::compile(const ShaderParsingResult &shader_sources) {
        int shader_program_id = glCreateProgram();
        int vertex_shader_id = 0;
        int fragment_shader_id = 0;
        int geometry_shader_id = 0;
        defer {
            glDeleteShader(vertex_shader_id);
            glDeleteShader(fragment_shader_id);
            glDeleteShader(geometry_shader_id);
        };

        vertex_shader_id = compile(shader_sources.vertex_shader, ShaderType::Vertex);
        glAttachShader(shader_program_id, vertex_shader_id);
        fragment_shader_id = compile(shader_sources.fragment_shader, ShaderType::Fragment);
        glAttachShader(shader_program_id, fragment_shader_id);

        if (!shader_sources.geometry_shader.empty()) {
            geometry_shader_id = compile(shader_sources.geometry_shader, ShaderType::Geometry);
            glAttachShader(shader_program_id, geometry_shader_id);
        }
        glLinkProgram(shader_program_id);
        ShaderProgram result(shader_program_id);
        return result;
    }

    int ShaderCompiler::compile(const std::string &shader_source, ShaderType type) {
        int shader_id = glCreateShader(to_opengl_type(type));
        const char *shader_source_cstr = shader_source.c_str();
        glShaderSource(shader_id, 1, &shader_source_cstr, nullptr);
        glCompileShader(shader_id);
        if (opengl_compilation_failed(shader_id)) {
            throw ShaderCompilationError(std::format("Shader compilation {} failed:\n{}", m_shader_name,
                                                     opengl_get_compilation_error_message(shader_id)));
        }
        return shader_id;
    }

    static std::string *get_current_shader(ShaderParsingResult &result, const std::string &line);

    ShaderParsingResult ShaderCompiler::parse_source() {
        ShaderParsingResult parsing_result;
        std::istringstream ss(m_sources);
        std::string line;
        std::string *current_shader;
        while (std::getline(ss, line)) {
            if (line.starts_with("#shader")) {
                current_shader = get_current_shader(parsing_result, line);
                continue;
            }
            current_shader->append(line);
            current_shader->push_back('\n');
        }
        if (parsing_result.vertex_shader.empty() || parsing_result.fragment_shader.empty()) {
            throw ShaderCompilationError(std::format(
                    "Error compiling: {}. Source for vertex and fragment shader must be defined.", m_shader_name));
        }
        return parsing_result;
    }

    ShaderCompilationResult ShaderCompiler::compile_from_file(std::string shader_name,
                                                              const std::filesystem::path &path) {
        if (!exists(path)) {
            return FileNotFoundError(
                    path, std::format("Shader source file {} for shader {} not found.", path.string(), shader_name));
        }
        return compile_from_source(std::move(shader_name), rg::read_file(path));
    }


    std::string *get_current_shader(ShaderParsingResult &result, const std::string &line) {
        if (line.ends_with(to_string(ShaderType::Vertex))) {
            return &result.vertex_shader;
        }
        if (line.ends_with(to_string(ShaderType::Fragment))) {
            return &result.fragment_shader;
        }
        if (line.ends_with(to_string(ShaderType::Geometry))) {
            return &result.geometry_shader;
        }
        RG_SHOULD_NOT_REACH_HERE("Unknown type of shader prefix: {}. Did you mean: #shader {}|{}|{}", line,
                                 to_string(ShaderType::Vertex), to_string(ShaderType::Fragment),
                                 to_string(ShaderType::Geometry));
    }

    static std::string_view to_string(ShaderType type) {
        switch (type) {
        case ShaderType::Vertex: return "vertex";
        case ShaderType::Fragment: return "fragment";
        case ShaderType::Geometry: return "geometry";
        }
    }

    int to_opengl_type(ShaderType type) {
        switch (type) {
        case ShaderType::Vertex: return GL_VERTEX_SHADER;
        case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
        }
    }
}// namespace rg