//
// Created by matfrg on 11/1/25.
//
#include <MainController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/Model.hpp>
#include <spdlog/spdlog.h>

namespace my_project {
void my_project::MyController::initialize() { spdlog::info("MyController has been initialized"); }

bool my_project::MyController::loop() {
    // If the user presses the escape key the application should close
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).state() == engine::platform::Key::State::JustReleased) { return false; }
    // if (platform->key(engine::platform::KEY_ESCAPE).state_str() == "JustReleased") { return false; }
    return true;
}

void MyController::poll_events() {}

}