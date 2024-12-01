//
// Created by spaske on 20.4.24..
//

#ifndef MATF_RG_ENGINE_ENGINE_H
#define MATF_RG_ENGINE_ENGINE_H
#include <memory>
namespace rg {
    class EngineError;
    class UserError;
    class App {
    public:
        /**
         * This is the only function that the user calls.
         * @return 0 if app terminated normally, -1 otherwise.
         */
        static int run(int argc, char **argv);

        virtual ~App() = default;

    protected:
        virtual void initialize(int argc, char **argv) = 0;

        virtual void poll_events() = 0;

        virtual bool loop() = 0;

        virtual void update() = 0;

        virtual void draw() = 0;

        virtual void terminate() = 0;

        virtual void handle_error(const UserError &);

        void handle_error_(const EngineError &e);
    };

    /**
     * The user implements this function that instantiates users own implementation of the App.
     * This function is called in the App::run method.
     * @return
     */
    std::unique_ptr<App> create_app();
}// namespace rg

#endif//MATF_RG_ENGINE_ENGINE_H
