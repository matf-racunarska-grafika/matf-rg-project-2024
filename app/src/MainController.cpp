//
// Created by filip on 1/22/25.
//

#include <engine/platform/PlatformController.hpp>
#include <MainController.hpp>

#include <spdlog/spdlog.h>

namespace app{

    void MainController::initialize() {
        spdlog::info("MainController::intialize");
    }

    bool MainController::loop(){
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if(platform->key(engine::platform::KeyID::KEY_ESCAPE).is_down()){
            return false;
        }
        return true;

    }

}