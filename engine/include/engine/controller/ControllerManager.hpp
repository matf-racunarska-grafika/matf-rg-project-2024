
//

#ifndef MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
#define MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
#include <engine/controller/Controller.hpp>
#include <engine/util/Errors.hpp>
#include <source_location>
#include <type_traits>
#include <algorithm>
#include <unordered_set>

namespace rg {
    class ControllerManager {
        friend class App;

    public:
        static ControllerManager *instance();

        template<typename TController>
        static TController *get(std::source_location location = std::source_location::current()) {
            static_assert(std::is_base_of_v<Controller, TController>);
            auto manager                   = instance();
            static TController *controller = manager->create_if_absent<TController>();
            RG_GUARANTEE(controller->is_initialized(),
                         "Trying to get Controller: {} in file:{}:{}, before it's been initialized. Call ControllerManager::initialize_controllers first"
                         "first.",
                         controller->name(), location.file_name(), location.line());
            RG_GUARANTEE(manager->is_registered_controller(controller),
                         "Trying to get Controller: {} in file:{}:{}, before registering it. Call register_controller "
                         "first.",
                         controller->name(), location.file_name(), location.line());
            return controller;
        }

        template<typename TController>
        TController *register_controller(std::source_location location = std::source_location::current()) {
            static_assert(std::is_base_of_v<Controller, TController>);
            TController *controller = create_if_absent<TController>();
            RG_GUARANTEE(!m_controllers_initialized,
                         "Trying to register Controller: `{}` in file: {}:{} after initialize_controllers() have benn called. Please make sure to register a controller before calling initialize_controllers()",
                         controller->name(), location.file_name(), location.line());
            RG_GUARANTEE(!is_registered_controller(controller),
                         "Trying to register Controller: `{}` twice in file: {}:{}. Please make "
                         "sure that every Controller is registered exactly once.",
                         controller->name(), location.file_name(), location.line());
            m_controllers.push_back(controller);
            return controller;
        }

        void initialize();

        void poll_events();

        void terminate();

        void begin_frame();

        bool loop();

        void update();

        void draw();

        void end_frame();

        void top_sort();

        void top_sort_util(Controller *controller, std::vector<Controller *> &stack,
                           std::unordered_set<Controller *> &visited);

        bool dfs_visits_controller_on_path(Controller *current, std::unordered_set<Controller *> &visited,
                                           std::unordered_set<Controller *> &path);

        bool has_cycle(const std::vector<Controller *> &controllers);

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
        bool m_controllers_initialized{false};
    };
} // namespace rg
#endif//MATF_RG_PROJECT_CONTROLLERMANAGER_HPP
