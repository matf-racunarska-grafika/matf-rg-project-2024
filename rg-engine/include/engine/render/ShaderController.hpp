
//

#ifndef MATF_RG_PROJECT_SHADERCONTROLLER_HPP
#define MATF_RG_PROJECT_SHADERCONTROLLER_HPP

#include <memory>
#include <unordered_map>
#include <engine/render/Shader.hpp>

namespace rg {

    class ShaderController : public rg::Controller {
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
        std::unordered_map<ShaderName, std::unique_ptr<ShaderProgram>> m_shaders;
        std::filesystem::path m_engine_shaders_path;
    };
}// namespace rg
#endif//MATF_RG_PROJECT_SHADERCONTROLLER_HPP
