//

#ifndef MATF_RG_PROJECT_ERRORS_HPP
#define MATF_RG_PROJECT_ERRORS_HPP

#include <source_location>
#include <string>
#include <utility>
#include <format>

namespace engine::util {
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

    class EngineError final : public Error {
    public:
        enum class Type {
            Unimplemented,
            ShouldNotReachHere,
            GuaranteeViolation,
            FileNotFound,
            ConfigurationError,
            ShaderCompilationError,
            OpenGLError,
            AssetLoadingError,

            EngineErrorCount
        };

        static std::string_view type_string(Type error);

        EngineError(Type error_type, std::string message,
                    std::source_location location = std::source_location::current())
        : Error(std::move(message), std::move(location))
      , m_error(error_type) {
        }

        std::string report() const override;

    private:
        Type m_error;
    };

    class UserError : public Error {
    public:
        using Error::Error;
    };
} // namespace engine

#define RG_GUARANTEE(expr, msg, ...)                                                                                   \
    do {                                                                                                               \
        if (!(expr)) {                                                                                                 \
            throw engine::util::EngineError(engine::util::EngineError::Type::GuaranteeViolation, std::format(msg, ##__VA_ARGS__), std::source_location::current());            \
        }                                                                                                              \
    } while (0)
#define RG_SHOULD_NOT_REACH_HERE(msg, ...)                                                                             \
    do {                                                                                                               \
        throw engine::util::EngineError(engine::util::EngineError::Type::ShouldNotReachHere, std::format(msg, ##__VA_ARGS__), std::source_location::current());                \
    } while (0)
#define RG_UNIMPLEMENTED(msg, ...)                                                                                     \
    do {                                                                                                               \
        throw engine::util::EngineError(engine::util::EngineError::Type::Unimplemented, std::format(msg, ##__VA_ARGS__), std::source_location::current());                     \
    } while (0)

#define RG_ENGINE_ERROR(type, msg, ...) \
    do { \
        throw engine::util::EngineError(type, std::format(msg, ##__VA_ARGS__), std::source_location::current()); \
    } while(0)

#endif//MATF_RG_PROJECT_ERRORS_HPP
