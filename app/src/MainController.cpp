//
// Created by boce on 7/26/25.
//

#include "engine/platform/PlatformController.hpp"

#include <MainController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void MainController::initialize() { spdlog::info("Main controller successfully initialized"); }

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).state() == engine::platform::Key::State::Pressed) { return false; }
    return true;
}

}// app