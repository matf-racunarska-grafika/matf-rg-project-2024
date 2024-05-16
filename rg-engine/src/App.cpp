//
// Created by spaske on 20.4.24..
//
#include <spdlog/spdlog.h>

#include "engine/App.hpp"
#include "engine/Utils.hpp"

namespace rg {

    void App::initialize_() {
        spdlog::info("App::initialize_");
        initialize();
    }

    bool App::loop_() {
        // Engine preparation for a loop_ iteration
        spdlog::info("App::loop_");
        // User preparation for a loop iteration
        return loop();
    }

    void App::poll_events_() {
        spdlog::info("App::poll_events_");
    }

    void App::update_() {
        // Update rg-engine systems
        spdlog::info("App::update_");

        // Update user systems
        update();
    }

    void App::draw_() {
        // Preparation rg-engine does for a draw_
        spdlog::info("App::draw_");

        // User draw_
        draw();
    }

    void App::terminate_() {
        // Terminate user systems
        terminate();

        // Terminate rg-engine systems
        spdlog::info("App::terminate_");
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
        } catch (const rg::error::UserError &e) {
            app->handle_error(e);
            app->terminate_();
        } catch (const rg::error::EngineError &e) {
            app->handle_error_(e);
            app->terminate_();
        } catch (...) {
            spdlog::error("Unknown error encountered. Shutting down...");
            app->terminate_();
        }
        return 0;
    }

    void App::handle_error(const rg::error::UserError &e) {
        spdlog::error(e.report());
    }

    void App::handle_error_(const rg::error::EngineError &e) {
        spdlog::error(e.report());
    }

} // rg