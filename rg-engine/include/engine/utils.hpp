//
// Created by spaske00 on 13.5.24..
//

#ifndef MATF_RG_PROJECT_UTILS_HPP
#define MATF_RG_PROJECT_UTILS_HPP

#include <vector>
#include <source_location>
#include <memory>
#include <format>

namespace rg::utils {
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
    class Service {
    public:
        virtual bool initialize() {
            return true;
        }

        virtual void terminate() {
        }

        virtual bool loop() {
            return true;
        }

        virtual void update() {
        }

        virtual void poll_events() {
        }

        virtual std::string_view name() = 0;

        virtual ~Service() = default;

    private:
    };

    class ServiceProvider {
    public:
        static ServiceProvider *singleton();

        template<typename ServiceT>
        static ServiceT *get() {
            static std::unique_ptr<ServiceT> service = ServiceT::create();
            return service.get();
        }

        template<typename ServiceT>
        void register_service(std::source_location location = std::source_location::current()) {
            auto new_service = ServiceProvider::get<ServiceT>();
            for (auto existing_service: m_service_registry) {
                EngineError::guarantee(existing_service != new_service, std::format(
                        "Trying to register service: `{}` twice in file: {}:{}. Please make sure that every service is registered exactly once.",
                        new_service->name(), location.file_name(), location.line()));
            }
            m_service_registry.push_back(new_service);
        }

        bool initialize();

        void poll_events();

        void terminate();

        bool loop();

        void update();

    private:
        std::vector<Service *> m_service_registry;
    };


}

#endif //MATF_RG_PROJECT_UTILS_HPP
