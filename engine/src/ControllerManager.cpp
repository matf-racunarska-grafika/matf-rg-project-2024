#include <rg/util/Utils.hpp>
#include <algorithm>
#include <rg/controller/Controller.hpp>
#include <rg/controller/ControllerManager.hpp>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <vector>
#include <libs/assimp/code/AssetLib/Collada/ColladaHelper.h>

//
namespace rg::controller {

    ControllerManager *ControllerManager::instance() {
        static std::unique_ptr<ControllerManager> provider = std::make_unique<ControllerManager>();
        return provider.get();
    }

    void topological_sort(std::vector<Controller *> &controllers);

    void ControllerManager::initialize() {
        /* Set to true here to prevent any Controller calling register_controller during initialization */
        m_controllers_initialized = true;

        topological_sort(m_controllers);
        std::string order_string;
        order_string.reserve(m_controllers.size() * 32);
        for (auto controller: m_controllers) {
            spdlog::info("{}::initialize", controller->name());
            controller->initialize();
            controller->mark_initialized();
            order_string.push_back('\n');
            order_string.append(controller->name());
        }
        spdlog::info("Controllers are now initialized and they will execute in the following order: {}", order_string);
    }

    bool ControllerManager::loop() {
        for (auto controller: m_controllers) {
            if (controller->is_enabled() && !controller->loop()) {
                return false;
            }
        }
        return true;
    }

    void ControllerManager::poll_events() {
        for (auto controller: m_controllers) {
            if (controller->is_enabled()) {
                controller->poll_events();
            }
        }
    }

    void ControllerManager::update() {
        for (auto controller: m_controllers) {
            if (controller->is_enabled()) {
                controller->update();
            }
        }
    }

    void ControllerManager::draw() {
        for (auto controller: m_controllers) {
            if (controller->is_enabled()) {
                controller->draw();
            }
        }
    }

    void ControllerManager::terminate() {
        int size = static_cast<int>(m_controllers.size()) - 1;
        for (int i = std::max(size, 0); i >= 0; --i) {
            auto controller = m_controllers[i];
            controller->terminate();
            spdlog::info("{}::terminate", controller->name());
        }
    }

    void topological_sort_util(Controller *controller, std::vector<Controller *> &stack,
                               std::unordered_set<Controller *> &visited);

    bool dfs_visits_controller_on_path(Controller *current, std::unordered_set<Controller *> &visited,
                                       std::unordered_set<Controller *> &path);

    bool has_cycle(const std::vector<Controller *> &controllers);

    void topological_sort(std::vector<Controller *> &controllers) {
        RG_GUARANTEE(!has_cycle(controllers), "Controller graph has a cycle!");
        std::unordered_set<Controller *> visited;
        std::vector<Controller *> stack;
        /* Make topological sort stable with regard to registration order if no other dependency exists. */
        for (auto controller = controllers.rbegin(); controller != controllers.rend(); ++controller) {
            if (!visited.contains(*controller)) {
                topological_sort_util(*controller, stack, visited);
            }
        }
        RG_GUARANTEE(visited.size() == controllers.size(), "Not all controller were visited.");
        RG_GUARANTEE(stack.size() == controllers.size(), "Not the same size.");
        controllers.clear();
        /*
         * Move the controllers from the stack back into controllers parameter because topologial_sort
         * sorts in-place.
         */
        std::move(stack.rbegin(), stack.rend(), std::back_inserter(controllers));
    }

    auto topological_sort_util(Controller *controller, std::vector<Controller *> &stack,
                               std::unordered_set<Controller *> &visited) -> void {
        visited.emplace(controller);

        for (Controller *executes_after: controller->next()) {
            if (!visited.contains(executes_after)) {
                topological_sort_util(executes_after, stack, visited);
            }
        }

        stack.push_back(controller);
    }

    bool has_cycle(const std::vector<Controller *> &controllers) {
        std::unordered_set<Controller *> visited;
        std::unordered_set<Controller *> path;
        for (Controller *root: controllers) {
            if (!visited.contains(root) && dfs_visits_controller_on_path(root, visited, path)) {
                return true;
            }
        }
        return false;
    }

    bool dfs_visits_controller_on_path(Controller *current,
                                       std::unordered_set<Controller *> &visited,
                                       std::unordered_set<Controller *> &path) {
        visited.emplace(current);
        path.emplace(current);
        for (Controller *next: current->next()) {
            if (!visited.contains(next) && dfs_visits_controller_on_path(next, visited, path)) {
                return true;
            }
            if (path.contains(next)) {
                return true;
            }
        }
        path.erase(current);
        return false;
    }

} // namespace rg
