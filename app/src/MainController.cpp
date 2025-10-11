//
// Created by matfrg on 10/11/25.
//

#include "MainController.hpp"

#include "spdlog/spdlog.h"

#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>

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


}// namespace app
