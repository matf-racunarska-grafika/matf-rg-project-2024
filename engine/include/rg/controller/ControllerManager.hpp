
//

#ifndef MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
#define MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
#include <rg/controller/Controller.hpp>
#include <rg/util/Errors.hpp>
#include <source_location>
#include <type_traits>
#include <algorithm>
#include <unordered_set>

namespace rg::core {
    class App;
}

namespace rg::controller {
    /**
    * @class ControllerManager
    * @brief Manages the @ref Controller registration, access, and execution.
    *
    * The ControllerManager calls:
    *
    * @ref Controller::initialize
    *
    * @ref Controller::loop
    *
    * @ref Controller::poll_events
    *
    * @ref Controller::update
    *
    * @ref Controller::draw
    *
    * @ref Controller::terminate
    *
    * for every @ref Controller instance that has been registered via @ref ControllerManager::register_controller.
    * Every @ref Controller function in the list above is called in the corresponding @ref App function with the same name.
    *
    * Because of the dependencies between the controllers specified with
    * @ref Controller::before and @ref Controller::after we sort the controller instances
    * topologically in the @ref ControllerManager::initialize to guarantee the correct order of execution
    * and not have them depend on the order of registration.
    *
    * Example usage:
    * @code
    * void setup_(...) {
    *   auto graphics = rg::controller::register_controller<GraphicsController>();
    *   auto platform = rg::controller::register_controller<PlatformController>();
    *   // Specify that the PlatformController always executes before GraphicsController.
    *   platform->before(graphics);
    * }
    *
    * void draw() {
    *   auto graphics = rg::controller::get<GraphicsController>();
    *   graphics->begin_gui();
    *   ...
    *   graphics->end_gui();
    * }
    * @endcode
    */
    class ControllerManager {
        friend class rg::core::App;

    public:
        /**
        * @brief Serves as a single access point for all the Controller types throughout the code base.
        *
        * @returns The only instance of the TController.
        */
        template<typename TController>
        static TController *get(std::source_location location = std::source_location::current()) {
            static_assert(std::is_base_of_v<Controller, TController>);
            auto manager                   = instance();
            static TController *controller = manager->create_if_absent<TController>();
            RG_GUARANTEE(controller->is_registered(),
                         "Trying to get an unregistered controller in: {}:{}.\nPlease call register_controller<> first.",
                         location.file_name(), location.line());
            return controller;
        }

        /**
        * @brief Registers the controller for execution.
        * The Controller instance that the register_controller returns isn't yet initialized.
        * It will be initialized during the @ref App::initialize.
        * If the register_controller is called twice for the same controller, it's registered only once.
        * The other calls just return the pointer to the already registered instance.
        * @returns Pointer to the only instance of the provided Controller class TController.
        */
        template<typename TController>
        static TController *register_controller(std::source_location location = std::source_location::current()) {
            static_assert(std::is_base_of_v<Controller, TController>);
            auto manager            = instance();
            TController *controller = manager->create_if_absent<TController>();
            RG_GUARANTEE(!manager->m_controllers_initialized,
                         "Trying to register Controller: `{}` in file: {}:{} after initialize_controllers() have benn called. Please make sure to register controllers in the setup() phase.",
                         controller->name(), location.file_name(), location.line());
            if (!controller->is_registered()) {
                manager->m_controllers.push_back(controller);
                controller->mark_as_registered();
            }
            return controller;
        }

    private:
        static ControllerManager *instance();

        /**
        * @brief Topologically sorts and initializes Controllers based on the @ref Controller::before and @ref Controller::after relationships specified in the @ref App::setup.
        * @brief Calls @ref Controller::initialize for every registered controller instance where @ref Controller::is_enabled is true.
        *
        */
        void initialize();

        /**
        * @brief Call @ref Controller::poll_events for every registered controller instance where @ref Controller::is_enabled is true.
        */
        void poll_events();

        /**
        * @brief Call @ref Controller::loop for every registered controller instance where @ref Controller::is_enabled is true.
        * The main loop exits as soon as one of the enabled Controllers returns false in the @ref Controller::loop.
        * @returns false for the first registered controller that returned false, otherwise true and the loop continues.
        */
        bool loop();

        /**
        * @brief Call @ref Controller::update for every registered controller instance where @ref Controller::is_enabled is true.
        */
        void update();

        /**
        * @brief Call @ref Controller::begin_draw for every registered controller instance where @ref Controller::is_enabled is true.
        */
        void begin_draw();

        /**
        * @brief Call @ref Controller::draw for every registered controller instance where @ref Controller::is_enabled is true.
        */
        void draw();

        /**
        * @brief Call @ref Controller::end_draw for every registered controller instance where @ref Controller::is_enabled is true.
        */
        void end_draw();

        /**
        * @brief Call @ref Controller::terminate for every registered controller instance in the reverse order of initialization.
        */
        void terminate();

        /**
         * @brief Creates an TController instance if it doesn't exist.
         * @returns A pointer to the only instance of the TController.
         */
        template<typename TController>
        TController *create_if_absent() {
            /*
             * Local static variables are initialized only once when the execution first reaches their definition.
             * The next time the execution reaches already initialized local static variables it does nothing.
             */
            static std::unique_ptr<TController> controller = std::make_unique<TController>();
            return controller.get();
        }

        /**
         * @brief Store all the registered_controllers in a vector so that they are easier to iterate over.
         */
        std::vector<Controller *> m_controllers;

        /**
         * @brief Internal variable guaranteeing that no controller is registered after @ref ControllerManager::initialized.
         */
        bool m_controllers_initialized{false};
    };

    /**
    * @brief Shorthand function for @ref ControllerManager::get.
    * @returns The only instance of the TController.
    */
    template<typename TController>
    TController *get(std::source_location location = std::source_location::current()) {
        return ControllerManager::get<TController>(location);
    }

    /**
    * @brief Shorthand function for @ref ControllerManager::register_controller.
    * @returns Pointer to the only instance of the provided Controller class TController.
    */
    template<typename TController>
    TController *register_controller(std::source_location location = std::source_location::current()) {
        return ControllerManager::register_controller<TController>(location);
    }
} // namespace rg
#endif//MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
