//
// Created by spaske00 on 13.5.24..
//

#include "engine/utils.hpp"
#include "engine/platform.hpp"
#include <spdlog/spdlog.h>
#include <utility>

namespace rg {
    void guarantee(bool expr, std::string_view msg, std::source_location source_location) {
        if (!expr) {
            throw GuaranteeViolation(msg, source_location);
        }
    }

    void should_not_reach_here(std::string_view msg, std::source_location source_location) {
        throw ShouldNotReachHere(msg, source_location);
    }

    void unimplemented(std::string_view msg, std::source_location source_location) {
        throw Unimplemented(msg, source_location);
    }

    std::string Unimplemented::report() const {
        return std::format("Unimplemented: {}:{}. {}. This error should be used only as a placeholder. Ensure that the "
                           "code path is implemented or use ShouldNotReachHere.",
                           location().file_name(), location().line(), message());
    }

    std::string ShouldNotReachHere::report() const {
        return std::format("ShouldNotReach: {}:{}. {}. There is a logic error in the program. The execution should "
                           "never reach this point.",
                           location().file_name(), location().line(), message());
    }

    std::string GuaranteeViolation::report() const {
        return std::format("GuaranteeViolation in: {}:{}. {}. There is a logic error in the program. Please ensure "
                           "that the Guarantee is never violated.",
                           location().file_name(), location().line(), message());
    }

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
}// namespace rg
