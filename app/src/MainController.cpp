//
// Created by aleksa on 1.6.25..
//

#include "MainController.h"

#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>

#include "spdlog/spdlog.h"

namespace app {
    void MainController::initialize() {
        spdlog::info("MainController initialized");
    };
    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }


} // app