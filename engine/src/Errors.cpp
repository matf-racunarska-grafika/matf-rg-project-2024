//

#include <engine/util/Errors.hpp>
#include <format>
#include <spdlog/spdlog.h>

namespace engine::util {
    static bool g_tracing = true;

    void tracing_on() {
        g_tracing = true;
    }

    void tracing_off() {
        g_tracing = false;
    }

    void trace(std::source_location location) {
        if (g_tracing) {
            spdlog::info("{}, in {}:{}", location.function_name(), location.file_name(), location.line());
        }
    }

    std::string Unimplemented::report() const {
        return std::format("Unimplemented: {}:{}. {}. This error should be used only as a placeholder. Ensure that the "
                           "code path is implemented or use ShouldNotReachHere.",
                           location().file_name(), location().line(), message());
    }

    std::string ShouldNotReachHere::report() const {
        return std::format("ShouldNotReach: {}:{}. {}. There is a logic error in the program. The execution should "
                           "never reach this point.",
                           location().file_name(), location().line(), message());
    }

    std::string GuaranteeViolation::report() const {
        return std::format("GuaranteeViolation in: {}:{}.\n{}\nThere is a logic error in the program. Please ensure "
                           "that the Guarantee is never violated.",
                           location().file_name(), location().line(), message());
    }

    std::string FileNotFoundError::report() const {
        return std::format("FileNotFound {}:{}. {}. The file: \"{}\" is necessary for program's execution, please "
                           "ensure that the file is available.",
                           location().file_name(), location().line(), message(), file_path().string());
    }

    const std::filesystem::path &FileNotFoundError::file_path() const {
        return m_path;
    }

    std::string ShaderCompilationError::report() const {
        return std::format("ShaderCompilationError: {}:{}.\n{}", location().file_name(), location().line(), message());
    }

    std::string AssetLoadingError::report() const {
        return std::format("AssetLoadingError: {}:{}.\n{}\n Failed to load model {} from path: {}\n",
                           location().file_name(), location().line(), message(), m_model_name, m_path.string());
    }
} // namespace engine
