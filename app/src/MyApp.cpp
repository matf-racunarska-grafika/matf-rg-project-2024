//
// Created by aleksa on 30.5.25..
//

#include "MyApp.h"

#include "spdlog/spdlog.h"

namespace app {
    void MyApp::app_setup() {
        spdlog::info("App setup");
        spdlog::info("TEST");
    }
} // app