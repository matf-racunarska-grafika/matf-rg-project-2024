//
// Created by spaske00 on 13.5.24..
//

#include "engine/utils.hpp"
#include "engine/platform.hpp"
#include <spdlog/spdlog.h>
#include <utility>

namespace rg::utils {
    void error::guarantee(bool expr, std::string_view msg, std::source_location source_location) {
        if (!expr) {
            throw GuaranteeViolation(msg, source_location);
        }
    }

    void error::should_not_reach_here(std::string_view msg, std::source_location source_location) {
        throw ShouldNotReachHere(msg, source_location);
    }

    void error::unimplemented(std::string_view msg, std::source_location source_location) {
        throw Unimplemented(msg, source_location);
    }

    std::string error::Unimplemented::report() const {
        return std::format(
                "Unimplemented: {}:{}. {}. This error should be used only as a placeholder. Ensure that the code path is implemented or use error::ShouldNotReachHere.",
                location().file_name(), location().line(), message());
    }

    std::string error::ShouldNotReachHere::report() const {
        return std::format(
                "ShouldNotReach: {}:{}. {}. There is a logic error in the program. The execution should never reach this point.",
                location().file_name(), location().line(), message());
    }

    std::string error::GuaranteeViolation::report() const {
        return std::format(
                "GuaranteeViolation in: {}:{}. {}. There is a logic error in the program. Please ensure that the Guarantee is never violated.",
                location().file_name(), location().line(), message());
    }

    ServiceProvider *ServiceProvider::singleton() {
        static std::unique_ptr<ServiceProvider> provider = std::make_unique<ServiceProvider>();
        return provider.get();
    }

    bool ServiceProvider::initialize() {
        for (auto service: m_service_registry) {
            if (!service->initialize()) {
                spdlog::error("Service {:} failed to initialize!", service->name());
                return false;
            }
            spdlog::info("{}::initialize", service->name());
        }
        return true;
    }

    void ServiceProvider::terminate() {
        int size = (int) m_service_registry.size() - 1;
        for (int i = std::max(size, 0); i >= 0; --i) {
            auto service = m_service_registry[i];
            service->terminate();
            spdlog::info("{}::terminate", service->name());
        }
    }

    bool ServiceProvider::loop() {
        for (auto service: m_service_registry) {
            if (!service->loop()) {
                return false;
            }
        }
        return true;
    }

    void ServiceProvider::update() {
        for (auto service: m_service_registry) {
            service->update();
        }
    }

    void ServiceProvider::poll_events() {
        for (auto service: m_service_registry) {
            service->poll_events();
        }
    }
}
