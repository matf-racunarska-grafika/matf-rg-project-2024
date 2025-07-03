//
// Created by teodora on 4.7.25..
//

#include <MyApp.hpp>

#include <spdlog/spdlog.h>

namespace app {
    void MyApp::app_setup() {
        spdlog::info("App setup completed!");
    }
} // app