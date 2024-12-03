#include <engine/util/Utils.hpp>
#include <algorithm>
#include <engine/controller/Controller.hpp>
#include <engine/controller/ControllerManager.hpp>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <vector>

//
namespace rg {

    ControllerManager *ControllerManager::instance() {
        static std::unique_ptr<ControllerManager> provider = std::make_unique<ControllerManager>();
        return provider.get();
    }

    void ControllerManager::initialize() {
        top_sort();
        for (auto controller: m_controllers) {
            spdlog::info("{}::initialize", controller->name());
            controller->initialize();
            controller->mark_initialized();
        }
        m_controllers_initialized = true;
    }

    void ControllerManager::terminate() {
        int size = static_cast<int>(m_controllers.size()) - 1;
        for (int i = std::max(size, 0); i >= 0; --i) {
            auto controller = m_controllers[i];
            controller->terminate();
            spdlog::info("{}::terminate", controller->name());
        }
    }

    void ControllerManager::begin_frame() {
        for (auto controller: m_controllers) {
            controller->begin_frame();
        }
    }

    bool ControllerManager::loop() {
        for (auto controller: m_controllers) {
            if (!controller->loop()) {
                return false;
            }
        }
        return true;
    }

    void ControllerManager::poll_events() {
        for (auto controller: m_controllers) {
            controller->poll_events();
        }
    }

    void ControllerManager::update() {
        for (auto controller: m_controllers) {
            controller->update();
        }
    }

    void ControllerManager::draw() {
        for (auto controller: m_controllers) {
            controller->draw();
        }
    }

    void ControllerManager::end_frame() {
        for (auto controller: m_controllers) {
            controller->end_frame();
        }
    }

    void ControllerManager::top_sort() {
        RG_GUARANTEE(!has_cycle(m_controllers), "Controller graph has a cycle!");
        std::unordered_set<Controller *> visited;
        std::vector<Controller *> stack;
        for (auto controller: m_controllers) {
            if (!visited.contains(controller)) {
                top_sort_util(controller, stack, visited);
            }
        }
        RG_GUARANTEE(visited.size() == m_controllers.size(), "Not all controller were visited.");
        RG_GUARANTEE(stack.size() == m_controllers.size(), "Not the same size.");

        std::reverse(std::begin(stack), std::end(stack));
        m_controllers = std::move(stack);
    }

    void ControllerManager::top_sort_util(Controller *controller, std::vector<Controller *> &stack,
                                          std::unordered_set<Controller *> &visited) {
        visited.emplace(controller);

        for (Controller *executes_after: controller->next()) {
            if (!visited.contains(executes_after)) {
                top_sort_util(executes_after, stack, visited);
            }
        }

        stack.push_back(controller);
    }

    bool ControllerManager::has_cycle(const std::vector<Controller *> &controllers) {
        std::unordered_set<Controller *> visited;
        std::unordered_set<Controller *> path;
        for (Controller *root: controllers) {
            if (!visited.contains(root)) {
                if (dfs_visits_controller_on_path(root, visited, path)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool ControllerManager::dfs_visits_controller_on_path(Controller *current,
                                                          std::unordered_set<Controller *> &visited,
                                                          std::unordered_set<Controller *> &path) {
        visited.emplace(current);
        path.emplace(current);
        for (Controller *next: current->next()) {
            if (!visited.contains(next) && dfs_visits_controller_on_path(next, visited, path)) {
                return true;
            } else if (path.contains(next)) {
                return false;
            }
        }
        path.erase(current);
        return false;
    }
} // namespace rg
