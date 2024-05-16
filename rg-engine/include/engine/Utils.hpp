//
// Created by spaske00 on 13.5.24..
//

#ifndef MATF_RG_PROJECT_UTILS_HPP
#define MATF_RG_PROJECT_UTILS_HPP

#include <source_location>
#include <memory>

namespace rg {

    namespace error {
        void guarantee(bool expr, std::string_view msg,
                       std::source_location source_location = std::source_location::current());

        void should_not_reach_here(std::string_view msg,
                                   std::source_location source_location = std::source_location::current());

        void
        unimplemented(std::string_view msg, std::source_location source_location = std::source_location::current());

        class Error : public std::exception {
        public:
            explicit Error(std::string_view message, std::source_location location = std::source_location::current())
                    : m_message(message), m_location(location) {
            }

            std::string_view message() const {
                return m_message;
            }

            std::source_location location() const {
                return m_location;
            }

            virtual std::string report() const = 0;

        private:
            std::string_view m_message;
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

        class UserError : public Error {
        public:
            using Error::Error;
        };
    };

}


#endif //MATF_RG_PROJECT_UTILS_HPP
