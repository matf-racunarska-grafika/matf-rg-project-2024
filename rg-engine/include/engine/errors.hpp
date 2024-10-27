//

#ifndef MATF_RG_PROJECT_ERRORS_HPP
#define MATF_RG_PROJECT_ERRORS_HPP

#include <string>
#include <source_location>

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

    class Error : public std::exception {
    public:
        explicit Error(std::string message, std::source_location location = std::source_location::current())
                : m_message(std::move(message)), m_location(location) {
        }

        const std::string &message() const {
            return m_message;
        }

        std::source_location location() const {
            return m_location;
        }

        virtual std::string report() const = 0;

    private:
        std::string m_message;
        std::source_location m_location;
    };

    class EngineError : public Error {
    public:
        using Error::Error;
    };

    class Unimplemented : public EngineError {
    public:
        using EngineError::EngineError;

        std::string report() const override;
    };

    class ShouldNotReachHere : public EngineError {
    public:
        using EngineError::EngineError;

        std::string report() const override;
    };

    class GuaranteeViolation : public EngineError {
    public:
        using EngineError::EngineError;

        std::string report() const override;
    };

    class FileNotFoundError : public EngineError {
    public:
        explicit FileNotFoundError(std::string_view path, std::string message,
                                   std::source_location location = std::source_location::current()) : m_path(path),
                                                                                                      EngineError(
                                                                                                              std::move(
                                                                                                                      message),
                                                                                                              location) {
        }

        std::string report() const override;

        std::string_view file_path() const;

    private:
        std::string_view m_path;
    };

    class ConfigurationError : public EngineError {
    public:
        using EngineError::EngineError;

        std::string report() const override;
    };


    class UserError : public Error {
    public:
        using Error::Error;
    };

} // rg

#endif //MATF_RG_PROJECT_ERRORS_HPP
