//
// Created by ana on 5/13/25.
//

#include "../include/MainController.hpp"

#include <engine/platform/PlatformController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void MainController::initialize() { spdlog::info("MainController initialized!"); }

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
    return true;


}
}// app