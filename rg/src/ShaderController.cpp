
//
#include <engine/controller/ControllerManager.hpp>
#include <engine/render/ShaderController.hpp>
#include <engine/util/Utils.hpp>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <variant>

namespace rg {

    void ShaderController::initialize() {
        const auto &config = Configuration::config();

        // set engine shaders path
        {
            const std::filesystem::path engine_shaders_path(config["shaders"]["path"]);
            m_shaders_path = engine_shaders_path / shader_language();
        }

        for (const auto &shader_path : std::filesystem::directory_iterator(m_shaders_path)) {
            const auto name = shader_path.path().stem().string();
            ShaderCompilationResult compilation_result = ShaderCompiler::compile_from_file(name, shader_path);
            std::visit(overloaded{[&](ShaderProgram shader_program) {
                                      m_shaders[name] = std::make_unique<ShaderProgram>(shader_program);
                                  },
                                  [](const ShaderCompilationError &error) { spdlog::error(error.report()); },
                                  [](const FileNotFoundError &error) { spdlog::error(error.report()); }},
                       compilation_result);
        }
    }

    std::string_view ShaderController::shader_language() const {
        return "glsl";
    }
}// namespace rg