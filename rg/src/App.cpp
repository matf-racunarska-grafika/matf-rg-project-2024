//
// Created by spaske00 on 20.4.24..
//
#include <spdlog/spdlog.h>
#include <engine/App.hpp>
#include <engine/util/Errors.hpp>

namespace rg {
    int App::run(int argc, char **argv) {
        auto app = create_app();
        try {
            app->initialize(argc, argv);
            while (app->loop()) {
                app->poll_events();
                app->update();
                app->draw();
            }
            app->terminate();
        } catch (const EngineError &e) {
            app->handle_error_(e);
            app->terminate();
            return -1;
        } catch (const UserError &e) {
            app->handle_error(e);
            app->terminate();
            return -1;
        } catch (const std::exception &e) {
            spdlog::error(e.what());
            app->terminate();
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