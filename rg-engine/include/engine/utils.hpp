//
// Created by spaske00 on 13.5.24..
//

#ifndef MATF_RG_PROJECT_UTILS_HPP
#define MATF_RG_PROJECT_UTILS_HPP

#include <format>
#include <memory>
#include <source_location>
#include <vector>


namespace rg {

    void guarantee(bool expr, std::string_view msg,
                   std::source_location source_location = std::source_location::current());

    void should_not_reach_here(std::string_view msg,
                               std::source_location source_location = std::source_location::current());

    void unimplemented(std::string_view msg, std::source_location source_location = std::source_location::current());


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

    class Controller {
        friend class ControllerManager;

    public:
        virtual std::string_view name() const = 0;

        virtual ~Controller() = default;

    private:
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
    };

    class ControllerManager {
        friend class App;

    public:
        static ControllerManager *singleton();

        template<typename TController>
        static TController *get() {
            static std::unique_ptr<TController> controller = TController::create();
            return controller.get();
        }

        template<typename TController>
        void register_controller(std::source_location location = std::source_location::current()) {
            auto new_controller = ControllerManager::get<TController>();
            for (auto existing_controller: m_controllers) {
                rg::guarantee(existing_controller != new_controller,
                              std::format("Trying to register Controller: `{}` twice in file: {}:{}. Please make "
                                          "sure that every Controller is registered exactly once.",
                                          new_controller->name(), location.file_name(), location.line()));
            }
            m_controllers.push_back(new_controller);
        }

    private:
        bool initialize();

        void poll_events();

        void terminate();

        bool loop();

        void update();

        std::vector<Controller *> m_controllers;
    };

}// namespace rg

#endif//MATF_RG_PROJECT_UTILS_HPP
