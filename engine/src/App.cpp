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
            setup_(argc, argv);
            initialize_();
            while (loop_()) {
                poll_events_();
                begin_frame_();
                update_();
                draw_();
                end_frame_();
            }
            terminate_();
        } catch (const EngineError &e) {
            handle_error_(e);
            terminate_();
            return -1;
        } catch (const UserError &e) {
            handle_error(e);
            terminate_();
            return -1;
        } catch (const std::exception &e) {
            handle_error_(e);
            terminate_();
        }
        return on_exit_();
    }

    void App::setup_(int argc, char **argv) {
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

    void App::initialize_() {
        ControllerManager::instance()->initialize();
        initialize();
    }

    bool App::loop_() {
        /*
            * Any controller can stop the rendering loop.
            */
        if (!ControllerManager::instance()->loop()) {
            return false;
        }
        return loop();
    }

    void App::poll_events_() {
        ControllerManager::instance()->poll_events();
        poll_events();
    }

    void App::begin_frame_() {
        ControllerManager::instance()->begin_frame();
        begin_frame();
    }

    void App::update_() {
        ControllerManager::instance()->update();
        update();
    }

    void App::draw_() {
        ControllerManager::instance()->draw();
        draw();
    }

    void App::end_frame_() {
        end_frame();
        ControllerManager::instance()->end_frame();
    }

    void App::terminate_() {
        terminate();
        ControllerManager::instance()->end_frame();
    }

    int App::on_exit_() {
        return on_exit();
    }

    void App::handle_error_(const std::exception &exception) {
        spdlog::error("std::exception occurred: {}", exception.what());
    }

    void App::handle_error(const UserError &e) {
        spdlog::error(e.report());
    }

    void App::handle_error_(const EngineError &e) {
        spdlog::error("EngineError: {}. This Error isn't recoverable, it indicates an error in the programs logic.",
                      e.report());
    }

} // namespace rg
