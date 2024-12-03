//
// Created by spaske on 20.4.24..
//

#ifndef MATF_RG_ENGINE_ENGINE_H
#define MATF_RG_ENGINE_ENGINE_H

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
        void setup_(int argc, char **argv);

        void initialize_();

        void poll_events_();

        bool loop_();

        void begin_frame_();

        void update_();

        void draw_();

        void end_frame_();

        void terminate_();

        int on_exit_();

        void handle_error_(const std::exception &exception);

        void handle_error_(const EngineError &e);

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

        virtual void begin_frame() {
            // Intentionally blank because it doesn't have to be overridden.
        };

        virtual void update() {
            // Intentionally blank because it doesn't have to be overridden.
        };

        virtual void draw() {
            // Intentionally blank because it doesn't have to be overridden.
        };

        virtual void end_frame() {
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

#endif//MATF_RG_ENGINE_ENGINE_H
