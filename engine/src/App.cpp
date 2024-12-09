//
// Created by spaske00 on 20.4.24
//
#include <spdlog/spdlog.h>
#include <engine/core/App.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/util/Errors.hpp>
#include <engine/util/Utils.hpp>
#include <engine/controller/ControllerManager.hpp>
#include <engine/controller/EngineSentinelController.hpp>
#include <engine/util/ArgParser.hpp>
#include <engine/util/Configuration.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace engine::core {
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
        } catch (const util::Error &e) {
            handle_error(e);
            terminate();
        }
        return on_exit();
    }

    void App::setup_(int argc, char **argv) {
        util::ArgParser::instance()->initialize(argc, argv);
        util::Configuration::instance()->initialize();

        // register engine controller
        auto platform  = controller::ControllerManager::register_controller<platform::PlatformController>();
        auto graphics  = controller::ControllerManager::register_controller<graphics::GraphicsController>();
        auto resources = controller::ControllerManager::register_controller<resources::ResourcesController>();
        auto sentinel  = controller::ControllerManager::register_controller<controller::EngineSentinelController>();
        platform->before(graphics);
        graphics->before(resources);
        resources->before(sentinel);
        setup();
    }

    void App::initialize() {
        controller::ControllerManager::instance()->initialize();
    }

    bool App::loop() {
        if (!controller::ControllerManager::instance()->loop()) {
            return false;
        }
        return true;
    }

    void App::poll_events() {
        controller::ControllerManager::instance()->poll_events();
    }

    void App::update() {
        controller::ControllerManager::instance()->update();
    }

    void App::draw() {
        controller::ControllerManager::instance()->begin_draw();
        controller::ControllerManager::instance()->draw();
        controller::ControllerManager::instance()->end_draw();
    }

    void App::terminate() {
        controller::ControllerManager::instance()->terminate();
    }

    void App::setup() {
        RG_UNIMPLEMENTED("You should override App::setup in your App implementation.");
    }

    void App::handle_error(const util::Error &e) {
        spdlog::error(e.report());
    }

} // namespace engine

