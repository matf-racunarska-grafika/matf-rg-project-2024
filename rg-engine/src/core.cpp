//
// Created by spaske00 on 20.5.24..
//

#include <spdlog/spdlog.h>

#include "engine/core.hpp"


namespace rg {
    ControllerManager *ControllerManager::singleton() {
        static std::unique_ptr<ControllerManager> provider = std::make_unique<ControllerManager>();
        return provider.get();
    }

    void ControllerManager::initialize() {
        for (auto controller: m_controllers) {
            controller->initialize();
            spdlog::info("{}::initialize", controller->name());
        }
    }

    void ControllerManager::terminate() {
        int size = (int) m_controllers.size() - 1;
        for (int i = std::max(size, 0); i >= 0; --i) {
            auto controller = m_controllers[i];
            controller->terminate();
            spdlog::info("{}::terminate", controller->name());
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

    void ControllerManager::update() {
        for (auto controller: m_controllers) {
            controller->update();
        }
    }

    void ControllerManager::poll_events() {
        for (auto controller: m_controllers) {
            controller->poll_events();
        }
    }
}