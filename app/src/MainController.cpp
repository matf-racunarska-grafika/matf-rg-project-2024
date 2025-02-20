#include <memory>
#include <spdlog/spdlog.h>
#include <engine/core/Engine.hpp>
#include <MainController.hpp>

namespace app {
    void MainController::initialize() {
        spdlog::info("MainController initialized");
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }
} // app