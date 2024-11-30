//
// Created by spaske00 on 20.4.24..
//
#include <spdlog/spdlog.h>

#include <engine/App.hpp>
#include <engine/Core.hpp>
#include <engine/controller/ControllerManager.hpp>
#include <engine/platform/Platform.hpp>
#include <engine/render/ShaderController.hpp>
#include <engine/ecs/EntityController.hpp>
#include <engine/util/Utils.hpp>
#include <engine/render/AssetsController.hpp>

namespace rg {
    void App::initialize_(int argc, char **argv) {
        spdlog::info("App::initialize_controllers::begin");
        ArgParser::instance()->initialize(argc, argv);
        Configuration::instance()->initialize();

        // register engine controller
        auto controller_manager = ControllerManager::singleton();
        controller_manager->initialize();

        auto platform_controller = controller_manager->register_engine_controller<PlatformController>();
        auto shader_controller = controller_manager->register_engine_controller<ShaderController>();
        auto entity_controller = controller_manager->register_engine_controller<EntityController>();
        auto assets_controller = controller_manager->register_engine_controller<AssetsController>();

        platform_controller->before(shader_controller);

        entity_controller->after(shader_controller);
        entity_controller->after(platform_controller);

        assets_controller->after(shader_controller);
        assets_controller->after(platform_controller);

        // User initialization
        initialize();
        /*
         * Controller initialization is done after user-defined App::initialize because
         * user can register custom services in App::initialize_controllers.
         */
        controller_manager->initialize_controllers();
        spdlog::info("App::initialize_controllers::end");
    }

    void App::after_initialize_() {
        spdlog::info("App::after_initialize::begin");
        after_initialize();
        spdlog::info("App::after_initialize::end");
    }


    bool App::loop_() {
        /*
         * Any controller can stop the rendering loop.
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
        ControllerManager::singleton()->update();
        update();
    }

    void App::draw_() {
        draw();
        ControllerManager::singleton()->draw();
    }

    void App::terminate_() {
        terminate();
        ControllerManager::singleton()->terminate();
    }

    int App::run(int argc, char **argv) {
        auto app = create_app();
        try {
            app->initialize_(argc, argv);
            app->after_initialize_();
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
        } catch (const std::exception &e) {
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