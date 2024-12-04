//
// Created by spaske00 on 20.4.24..
//
#include <spdlog/spdlog.h>
#include <engine/core/App.hpp>
#include <engine/platform/Platform.hpp>
#include <engine/resources/Resources.hpp>
#include <engine/util/Errors.hpp>
#include <engine/util/Utils.hpp>
#include <engine/controller/ControllerManager.hpp>

namespace rg {
    int App::run(int argc, char **argv) {
        try {
            internal_setup(argc, argv);
            internal_initialize();
            while (internal_loop()) {
                internal_poll_events();
                internal_update();
                internal_draw();
            }
            internal_terminate();
        } catch (const EngineError &e) {
            internal_handle_error(e);
            internal_terminate();
            return -1;
        } catch (const UserError &e) {
            handle_error(e);
            internal_terminate();
            return -1;
        } catch (const std::exception &e) {
            internal_handle_error(e);
            internal_terminate();
        }
        return internal_on_exit();
    }

    void App::internal_setup(int argc, char **argv) {
        ArgParser::instance()->initialize(argc, argv);
        Configuration::instance()->initialize();

        // register engine controller
        auto controller_manager = ControllerManager::instance();
        auto platform           = controller_manager->register_controller<PlatformController>();
        auto resources          = controller_manager->register_controller<ResourcesController>();
        auto sentinel           = controller_manager->register_controller<EngineControllersSentinel>();
        resources->after(platform);
        sentinel->after(platform);
        sentinel->after(resources);

        setup();
    }

    void App::internal_initialize() {
        ControllerManager::instance()->initialize();
        initialize();
    }

    bool App::internal_loop() {
        /*
            * Any controller can stop the rendering loop.
            */
        if (!ControllerManager::instance()->loop()) {
            return false;
        }
        return loop();
    }

    void App::internal_poll_events() {
        ControllerManager::instance()->poll_events();
        poll_events();
    }

    void App::internal_update() {
        ControllerManager::instance()->update();
        update();
    }

    void App::internal_draw() {
        ControllerManager::instance()->draw();
        draw();
    }

    void App::internal_terminate() {
        terminate();
    }

    int App::internal_on_exit() {
        return on_exit();
    }

    void App::internal_handle_error(const std::exception &exception) {
        spdlog::error("std::exception occurred: {}", exception.what());
    }

    void App::handle_error(const UserError &e) {
        spdlog::error(e.report());
    }

    void App::internal_handle_error(const EngineError &e) {
        spdlog::error("EngineError: {}. This Error isn't recoverable, it indicates an error in the programs logic.",
                      e.report());
    }

} // namespace rg
