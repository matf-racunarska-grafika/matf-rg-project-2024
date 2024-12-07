//

#ifndef MATF_RG_PROJECT_ERRORS_HPP
#define MATF_RG_PROJECT_ERRORS_HPP

#include <filesystem>
#include <format> //NOLINT
#include <source_location>
#include <string>
#include <utility>

#define RG_GUARANTEE(expr, msg, ...)                                                                                   \
    do {                                                                                                               \
        if (!(expr)) {                                                                                                 \
            throw rg::GuaranteeViolation(std::format(msg, ##__VA_ARGS__), std::source_location::current());            \
        }                                                                                                              \
    } while (0)
#define RG_SHOULD_NOT_REACH_HERE(msg, ...)                                                                             \
    do {                                                                                                               \
        throw rg::ShouldNotReachHere(std::format(msg, ##__VA_ARGS__), std::source_location::current());                \
    } while (0)
#define RG_UNIMPLEMENTED(msg, ...)                                                                                     \
    do {                                                                                                               \
        throw rg::Unimplemented(std::format(msg, ##__VA_ARGS__), std::source_location::current());                     \
    } while (0)

namespace rg {
    void tracing_on();

    void tracing_off();

    void trace(std::source_location location = std::source_location::current());

    class Error : public std::exception {
    public:
        explicit Error(std::string message, std::source_location location = std::source_location::current())
        : m_message(std::move(message))
      , m_location(location) {
        }

        const std::string &message() const {
            return m_message;
        }

        std::source_location location() const {
            return m_location;
        }

        virtual std::string report() const {
            return m_message;
        };

    private:
        std::string m_message;
        std::source_location m_location;
    };

    class EngineError : public Error {
    public:
        using Error::Error;
    };

    class Unimplemented final : public EngineError {
    public:
        using EngineError::EngineError;

        std::string report() const override;
    };

    class ShouldNotReachHere final : public EngineError {
    public:
        using EngineError::EngineError;

        std::string report() const override;
    };

    class GuaranteeViolation final : public EngineError {
    public:
        using EngineError::EngineError;

        std::string report() const override;
    };

    class FileNotFoundError final : public EngineError {
    public:
        explicit FileNotFoundError(std::filesystem::path path, std::string message,
                                   std::source_location location = std::source_location::current())
        : EngineError(std::move(message), location)
      , m_path(std::move(path)) {
        }

        std::string report() const override;

        const std::filesystem::path &file_path() const;

    private:
        std::filesystem::path m_path;
    };

    class ConfigurationError final : public EngineError {
    public:
        using EngineError::EngineError;

        std::string report() const override;
    };

    class UserError : public Error {
    public:
        using Error::Error;
    };

    class ShaderCompilationError final : public UserError {
    public:
        using UserError::UserError;

        std::string report() const override;
    };

    class AssetLoadingError final : public UserError {
    public:
        using UserError::UserError;

        AssetLoadingError(std::string message, std::filesystem::path path, std::string model_name,
                          std::source_location location = std::source_location::current())
        : UserError(std::move(message), location)
      , m_path(std::move(path))
      , m_model_name(std::move(model_name)) {
        }

        std::string report() const override;

    private:
        std::filesystem::path m_path;
        std::string m_model_name;
    };
} // namespace rg

#endif//MATF_RG_PROJECT_ERRORS_HPP
