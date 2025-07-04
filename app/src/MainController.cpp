//
// Created by teodora on 4.7.25..
//

#include "engine/platform/PlatformController.hpp"
#include "spdlog/spdlog.h"
#include <MainController.hpp>


namespace app {
void MainController::initialize() {
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}

}// namespace app