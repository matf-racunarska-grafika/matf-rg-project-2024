//
// Created by spaske00 on 20.4.24
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
            setup_(argc, argv);
            initialize();
            while (loop()) {
                poll_events();
                update();
                draw();
            }
            terminate();
        } catch (const Error &e) {
            handle_error(e);
            terminate();
        }
        return on_exit();
    }

    void App::setup_(int argc, char **argv) {
        ArgParser::instance()->initialize(argc, argv);
        Configuration::instance()->initialize();

        // register engine controller
        auto platform  = ControllerManager::register_controller<PlatformController>();
        auto resources = ControllerManager::register_controller<ResourcesController>();
        auto sentinel  = ControllerManager::register_controller<EngineControllersSentinel>();
        resources->after(platform);
        sentinel->after(platform);
        sentinel->after(resources);

        setup();
    }

    void App::initialize() {
        ControllerManager::instance()->initialize();
    }

    bool App::loop() {
        /*
            * Any controller can stop the rendering loop.
            */
        if (!ControllerManager::instance()->loop()) {
            return false;
        }
        return true;
    }

    void App::poll_events() {
        ControllerManager::instance()->poll_events();
    }

    void App::update() {
        ControllerManager::instance()->update();
    }

    void App::draw() {
        ControllerManager::instance()->draw();
    }

    void App::terminate() {
        ControllerManager::instance()->terminate();
    }

    void App::handle_error(const Error &e) {
        spdlog::error(e.report());
    }

} // namespace rg
