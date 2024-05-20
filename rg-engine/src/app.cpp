//
// Created by spaske00 on 20.4.24..
//
#include <spdlog/spdlog.h>

#include "engine/app.hpp"
#include "engine/platform.hpp"
#include "engine/utils.hpp"
#include "engine/core.hpp"

namespace rg {
    void App::initialize_() {
        Configuration::instance()->initialize();

        // register engine services
        auto controller_manager = ControllerManager::singleton();
        controller_manager->register_controller<rg::PlatformController>();
        controller_manager->register_controller<rg::WindowController>();
        controller_manager->register_controller<rg::InputController>();

        // User initialization
        initialize();
        /*
         * Controller initialization is done after user-defined App::initialize because
         * user can register custom services in App::initialize.
         */
        controller_manager->initialize();
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
        try {
            app->initialize_();
            while (app->loop_()) {
                app->poll_events_();
                app->update_();
                app->draw_();
            }
            app->terminate_();
        } catch (const EngineError &e) {
            app->handle_error_(e);
            app->terminate_();
            return -1;
        } catch (const UserError &e) {
            app->handle_error(e);
            app->terminate_();
            return -1;
        } catch (const std::exception& e) {
            spdlog::error(e.what());
            app->terminate_();
        }
        return 0;
    }

    void App::handle_error(const UserError &e) {
        spdlog::error(e.report());
    }

    void App::handle_error_(const EngineError &e) {
        spdlog::error("EngineError: {}. This Error isn't recoverable, it indicates an error in the programs logic.",
                      e.report());
    }


}// namespace rg