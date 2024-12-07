//
// Created by spaske on 20.4.24..
//

#ifndef APP_H
#define APP_H

namespace rg {
    class Error;

    class App {
    public:
        /**
         * This is the only function that the user calls.
         * @return 0 if app terminated normally, -1 otherwise.
         */
        int run(int argc, char **argv);

        virtual ~App() = default;

        static std::unique_ptr<App> create();

    private:
        void setup_(int argc, char **argv);

        void initialize();

        void poll_events();

        bool loop();

        void update();

        void draw();

        void terminate();

    protected:
        virtual void setup() {
            // Intentionally blank because it doesn't have to be overridden.
        }

        virtual int on_exit() {
            return 0;
        }

        virtual void handle_error(const Error &);
    };
} // namespace rg

#endif//APP_H
