//
// Created by spaske on 20.4.24..
//

#ifndef MATF_RG_internal_internal_H
#define MATF_RG_internal_internal_H
#include <engine/controller/ControllerManager.hpp>

namespace rg {
    class EngineError;
    class UserError;

    class App {
    public:
        /**
         * This is the only function that the user calls.
         * @return 0 if app terminated normally, -1 otherwise.
         */
        int run(int argc, char **argv);

        virtual ~App() = default;

    private:
        void internal_setup(int argc, char **argv);

        void internal_initialize();

        void internal_poll_events();

        bool internal_loop();

        void internal_update();

        void internal_draw();

        void internal_terminate();

        int internal_on_exit();

        void internal_handle_error(const std::exception &exception);

        void internal_handle_error(const EngineError &e);

    protected:
        virtual void setup() {
            // Intentionally blank because it doesn't have to be overridden.
        }

        virtual void initialize() {
            // Intentionally blank because it doesn't have to be overridden.
        };

        virtual void poll_events() {
            // Intentionally blank because it doesn't have to be overridden.
        };

        virtual bool loop() {
            // Intentionally blank because it doesn't have to be overridden.
            return true;
        };

        virtual void update() {
            // Intentionally blank because it doesn't have to be overridden.
        };

        virtual void draw() {
            // Intentionally blank because it doesn't have to be overridden.
        };

        virtual void terminate() {
            // Intentionally blank because it doesn't have to be overridden.
        };

        virtual int on_exit() {
            // Intentionally blank because it doesn't have to be overridden.
            return 0;
        }

        virtual void handle_error(const UserError &);
    };
} // namespace rg

#endif//MATF_RG_internal_internal_H
