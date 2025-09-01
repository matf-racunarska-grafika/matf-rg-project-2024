//
// Created by filipn on 9/1/25.
//

#include "../include/MainController.hpp"

#include <engine/platform/PlatformController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void MainController::initialize() { spdlog::info("MainController initialized!!!"); }

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).is_down()) { return false; }
    return true;
}
}