//
// Created by spaske00 on 20.4.24..
//
#include <spdlog/spdlog.h>

#include "engine/app.hpp"
#include "engine/platform.hpp"
#include "engine/utils.hpp"

namespace rg {
    using namespace rg::utils;
    using namespace rg::platform;

    bool App::initialize_() {

        // register engine services
        ServiceProvider::singleton()->register_service<rg::platform::Platform>();
        ServiceProvider::singleton()->register_service<rg::platform::Window>();
        ServiceProvider::singleton()->register_service<rg::platform::InputController>();

        if (!initialize()) {
            spdlog::error("initialize failed!");
            return false;
        }

        /*
         * Service initialization is done after user-defined App::initialize because
         * user can register custom services in App::initialize.
         */
        if (!rg::utils::ServiceProvider::singleton()->initialize()) {
            return false;
        }

        return true;
    }

    bool App::loop_() {
        /*
         * Any service can stop the rendering.
         */
        if (!rg::utils::ServiceProvider::singleton()->loop()) {
            return false;
        }

        return loop();
    }

    void App::poll_events_() {
        ServiceProvider::get<Platform>()->poll_events();
    }

    void App::update_() {
        rg::utils::ServiceProvider::singleton()->update();
        update();
    }

    void App::draw_() {
        draw();
    }

    void App::terminate_() {
        terminate();
        rg::utils::ServiceProvider::singleton()->terminate();
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