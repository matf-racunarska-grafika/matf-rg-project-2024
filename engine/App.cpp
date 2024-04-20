//
// Created by spaske on 20.4.24..
//
#include <spdlog/spdlog.h>

#include "App.hpp"

namespace rg {

    bool App::initialize_() {
        spdlog::info("App::initialize_");

        if (!initialize()) {
            spdlog::error("initialize failed!");
            return false;
        }

        return true;
    }

    bool App::loop_() {
        // Engine preparation for a loop_ iteration
        spdlog::info("App::loop_");

        // User preparation for a loop iteration
        return loop();
    }

    void App::poll_events_() {
        spdlog::info("App::poll_events_");
    }

    void App::update_() {
        // Update engine systems
        spdlog::info("App::update_");

        // Update user systems
        update();
    }

    void App::draw_() {
        // Preparation engine does for a draw_
        spdlog::info("App::draw_");

        // User draw_
        draw();
    }

    void App::terminate_() {
        // Terminate user systems
        terminate();

        // Terminate engine systems
        spdlog::info("App::terminate_");
    }


    int App::run() {
        auto app = create_app();

        if (!app->initialize_()) {
            spdlog::error("App failed to initialize! Shutdown...");
            app->terminate_();
            return -1;
        }

        while (app->loop_()) {
            app->poll_events_();
            app->update_();
            app->draw_();
        }

        app->terminate_();
        return 0;
    }


} // rg