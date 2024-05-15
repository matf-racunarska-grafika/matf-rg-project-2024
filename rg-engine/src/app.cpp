//
// Created by spaske00 on 20.4.24..
//
#include <spdlog/spdlog.h>

#include "engine/app.hpp"
#include "engine/platform.hpp"
#include "engine/utils.hpp"

namespace rg {
    bool App::initialize_() {

        // register engine services
        auto controller_manager = ControllerManager::singleton();
        controller_manager->register_controller<rg::PlatformController>();
        controller_manager->register_controller<rg::WindowController>();
        controller_manager->register_controller<rg::InputController>();

        if (!initialize()) {
            spdlog::error("initialize failed!");
            return false;
        }

        /*
         * Controller initialization is done after user-defined App::initialize because
         * user can register custom services in App::initialize.
         */
        if (!controller_manager->initialize()) {
            return false;
        }

        return true;
    }

    bool App::loop_() {
        /*
         * Any service can stop the rendering.
         */
        if (!rg::ControllerManager::singleton()->loop()) {
            return false;
        }

        return loop();
    }

    void App::poll_events_() {
        ControllerManager::singleton()->poll_events();
    }

    void App::update_() {
        rg::ControllerManager::singleton()->update();
        update();
    }

    void App::draw_() {
        draw();
    }

    void App::terminate_() {
        terminate();
        rg::ControllerManager::singleton()->terminate();
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