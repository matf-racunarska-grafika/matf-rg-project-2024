
//

#ifndef MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
#define MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
#include <engine/controller/Controller.hpp>
#include <engine/util/Errors.hpp>
#include <source_location>
#include <type_traits>
#include <algorithm>
namespace rg {
    class ControllerManager {
        friend class App;
    public:
        static ControllerManager *singleton();

        template<typename TController>
        static TController *get(std::source_location location = std::source_location::current()) {
            static_assert(std::is_base_of_v<Controller, TController>);
            auto instance = singleton();
            static TController *controller = singleton()->create_if_absent<TController>();
            RG_GUARANTEE(instance->is_registered_controller(controller),
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

        void initialize();

        void initialize_controllers();

        void poll_events();

        void terminate();

        bool loop();

        void update();

        void draw();

    private:
        template<typename TController>
        TController *create_if_absent() {
            static std::unique_ptr<TController> controller = TController::create();
            return controller.get();
        }

        bool is_registered_controller(Controller *controller) {
            return std::find(m_controllers.cbegin(), m_controllers.cend(), controller) != m_controllers.cend();
        }


        std::vector<Controller *> m_controllers;
    };
}// namespace rg
#endif//MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
