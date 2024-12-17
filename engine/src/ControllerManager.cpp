#include <engine/util/Utils.hpp>
#include <algorithm>
#include <engine/controller/Controller.hpp>
#include <engine/controller/ControllerManager.hpp>

#include <numeric>
#include <spdlog/spdlog.h>
#include <vector>

//
namespace engine::controller {

    ControllerManager *ControllerManager::instance() {
        static std::unique_ptr<ControllerManager> provider = std::make_unique<ControllerManager>();
        return provider.get();
    }

    void ControllerManager::initialize() {
        /* Set to true here to prevent any Controller calling register_controller during initialization */
        m_controllers_initialized = true;
        // Topologically sort controllers based on their dependency graph formed by before/after methods.
        {
            auto get_dependant_controllers = [](Controller *curr) {
                return curr->next();
            };
            RG_GUARANTEE(!util::alg::has_cycle(range(m_controllers), get_dependant_controllers),
                         "Please make sure that there are no cycles in the controller dependency graph.");
            util::alg::topological_sort(range(m_controllers), get_dependant_controllers);
        }
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
            controller->poll_events();
        }
    }

    void ControllerManager::update() {
        for (auto controller: m_controllers) {
            if (controller->is_enabled()) {
                controller->update();
            }
        }
    }

    void ControllerManager::begin_draw() {
        for (auto controller: m_controllers) {
            if (controller->is_enabled()) {
                controller->begin_draw();
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

    void ControllerManager::end_draw() {
        for (auto controller: m_controllers) {
            if (controller->is_enabled()) {
                controller->end_draw();
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
} // namespace engine
