//
// Created by bojana on 10/8/25.
//

#include "../include/MyController.hpp"

#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>

namespace app {
void MyController::initialize() {

}
bool MyController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}


}// namespace app