//
// Created by zesla on 10/9/25.
//

#include <MainController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <spdlog/spdlog.h>

#include <engine/platform/PlatformController.hpp>
namespace app {


    void app::MainController::initialize() {
        spdlog::info("KOntroler inicijalizovan");
    };
    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    }// namespace app;
