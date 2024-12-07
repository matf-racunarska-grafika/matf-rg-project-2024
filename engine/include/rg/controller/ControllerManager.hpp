
//

#ifndef MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
#define MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
#include <rg/controller/Controller.hpp>
#include <rg/util/Errors.hpp>
#include <source_location>
#include <type_traits>
#include <algorithm>
#include <unordered_set>

namespace rg {
    class App;
}

namespace rg::controller {
    class ControllerManager {
        friend class rg::App;

    public:
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

        template<typename TController>
        static TController *register_controller(std::source_location location = std::source_location::current()) {
            static_assert(std::is_base_of_v<Controller, TController>);
            auto manager            = instance();
            TController *controller = manager->create_if_absent<TController>();
            RG_GUARANTEE(!manager->m_controllers_initialized,
                         "Trying to register Controller: `{}` in file: {}:{} after initialize_controllers() have benn called. Please make sure to register controllers in the setup() phase.",
                         controller->name(), location.file_name(), location.line());
            RG_GUARANTEE(!manager->is_registered_controller(controller),
                         "Trying to register Controller: `{}` twice in file: {}:{}. Please make "
                         "sure that every Controller is registered exactly once.",
                         controller->name(), location.file_name(), location.line());
            manager->m_controllers.push_back(controller);
            controller->mark_as_registered();
            return controller;
        }

    private:
        static ControllerManager *instance();

        void initialize();

        void poll_events();

        bool loop();

        void update();

        void draw();

        void terminate();

        void topological_sort();

        void topological_sort_util(Controller *controller, std::vector<Controller *> &stack,
                                   std::unordered_set<Controller *> &visited);

        bool dfs_visits_controller_on_path(Controller *current, std::unordered_set<Controller *> &visited,
                                           std::unordered_set<Controller *> &path);

        bool has_cycle(const std::vector<Controller *> &controllers);

        template<typename TController>
        TController *create_if_absent() {
            static std::unique_ptr<TController> controller = std::make_unique<TController>();
            return controller.get();
        }

        bool is_registered_controller(Controller *controller) {
            return std::find(m_controllers.cbegin(), m_controllers.cend(), controller) != m_controllers.cend();
        }

        std::vector<Controller *> m_controllers;
        bool m_controllers_initialized{false};
    };

    template<typename TController>
    TController *get(std::source_location location = std::source_location::current()) {
        return ControllerManager::get<TController>(location);
    }

    template<typename TController>
    TController *register_controller(std::source_location location = std::source_location::current()) {
        return ControllerManager::register_controller<TController>(location);
    }
} // namespace rg
#endif//MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
