//
// Created by spaske00 on 20.5.24..
//

#ifndef MATF_RG_PROJECT_CORE_HPP
#define MATF_RG_PROJECT_CORE_HPP

#include <format>
#include <memory>
#include <source_location>
#include <string_view>
#include <vector>

#include "errors.hpp"
#include "utils.hpp"

namespace rg {

    class Controller {
        friend class ControllerManager;

    public:
        virtual std::string_view name() const = 0;

        virtual ~Controller() = default;

        /*
         * Make controller `next` execute next `this`.
         * c1->before(c2); means that all the controller functions
         * of c1 will execute next controller functions of c2;
         */
        void before(Controller *next) {
            m_next.push_back(next);
        }

        void after(Controller *prev) {
            prev->before(this);
        }

        const std::vector<Controller *> &next() const {
            return m_next;
        }

    private:
        virtual void initialize() {
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

        /* List of controllers that are dependent on this controller */
        std::vector<Controller *> m_next;
    };

    class ControllerManager {
        friend class App;

    public:
        static ControllerManager *singleton();

        template<typename TController>
        TController *get(std::source_location location = std::source_location::current()) {
            static_assert(std::is_base_of_v<Controller, TController>);
            static TController *controller = create_if_absent<TController>();
            RG_GUARANTEE(is_registered_controller(controller),
                         "Trying to get Controller: {} in file:{}:{}, before registering it. Call register_controller "
                         "first.",
                         controller->name(), location.file_name(), location.line());
            return controller;
        }

        template<typename TController>
        TController *register_controller(std::source_location location = std::source_location::current()) {
            static_assert(std::is_base_of_v<Controller, TController>);
            TController *controller = create_if_absent<TController>();
            RG_GUARANTEE(!is_registered_controller(controller),
                         "Trying to register Controller: `{}` twice in file: {}:{}. Please make "
                         "sure that every Controller is registered exactly once.",
                         controller->name(), location.file_name(), location.line());
            m_controllers.push_back(controller);
            return controller;
        }

    private:
        template<typename TController>
        TController *create_if_absent() {
            static std::unique_ptr<TController> controller = TController::create();
            return controller.get();
        }

        bool is_registered_controller(Controller *controller) {
            return std::find(m_controllers.cbegin(), m_controllers.cend(), controller) != m_controllers.cend();
        }


        void initialize();

        void poll_events();

        void terminate();

        bool loop();

        void update();

        std::vector<Controller *> m_controllers;
    };


}// namespace rg

#endif//MATF_RG_PROJECT_CORE_HPP
