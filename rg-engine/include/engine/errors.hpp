//

#ifndef MATF_RG_PROJECT_ERRORS_HPP
#define MATF_RG_PROJECT_ERRORS_HPP

#include <string>
#include <source_location>

namespace rg {
    void guarantee(bool expr, std::string msg, std::source_location source_location = std::source_location::current());

    void should_not_reach_here(std::string msg, std::source_location source_location = std::source_location::current());

    void unimplemented(std::string msg, std::source_location source_location = std::source_location::current());


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
