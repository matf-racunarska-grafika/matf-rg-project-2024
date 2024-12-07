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
#include <engine/controller/EngineSentinelController.hpp>

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
        auto platform  = controller::ControllerManager::register_controller<PlatformController>();
        auto resources = controller::ControllerManager::register_controller<ResourcesController>();
        auto sentinel  = controller::ControllerManager::register_controller<controller::EngineSentinelController>();
        resources->after(platform);
        sentinel->after(platform);
        sentinel->after(resources);

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
        controller::ControllerManager::instance()->draw();
    }

    void App::terminate() {
        controller::ControllerManager::instance()->terminate();
    }

    void App::setup() {
        RG_UNIMPLEMENTED("You should override App::setup in your App implementation.");
    }

    void App::handle_error(const Error &e) {
        spdlog::error(e.report());
    }

} // namespace rg

int main(int argc, char **argv) {
    return rg::App::create()->run(argc, argv);
}
