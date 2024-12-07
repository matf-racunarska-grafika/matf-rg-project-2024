/**
 * @file App.hpp
 * @brief Defines the base App class that serves as the application core structure and the entry point.
 */

#ifndef APP_H
#define APP_H

namespace rg {
    class Error;

    /**
    * @class App
    * @brief Defines the base App class that serves as the application core structure and the entry point.
    *
    * The main function calls @ref App::run that defines the core structure of the application.
    * You can hook into the engine execution by overriding the desired methods in your
    * implementations of the @ref rg::Controller class and registering your implementations
    * via @ref rg::ControllerManager.
    * @code
    * int App::run(int argc, char **argv) {
    *    try {
    *        setup_(argc, argv);
    *        initialize();
    *        while (loop()) {
    *            poll_events();
    *            update();
    *            draw();
    *        }
    *        terminate();
    *    } catch (const Error &e) {
    *        handle_error(e);
    *        terminate();
    *    }
    *    return on_exit();
    * }
    * @endcode
    */
    class App {
    public:
        /**
        * @brief The main entry point into the App.
        */
        int run(int argc, char **argv);

        /**
        * @brief Creates a new instance of the App. The user implements this function.
        *
        * To create your own app, you should inherit the App class and implement setup() and create() functions.
        * The function @ref App::create should return the instance of the user class.
        *
        * @code
        * class MyComputerGraphicsProjectApp : public rg::App {
        * public:
        *    void setup(int argc, char** argv) override {...}
        * };
        * namespace rg {
        *     std::unique_ptr<App> create() {
        *        return std::make_unique<MyComputerGraphicsProjectApp>();
        *     }
        * }
        * @endcode
        */
        static std::unique_ptr<App> create();

    private:
        /**
        * @brief The first function that the engine calls to do its internal Controller classes `setup_`.
        *
        * After the engine `setup_` is finished, `setup_` calls the @ref App::setup(int argc, char** argv) that
        * you can override and do your custom setup operations.
        * This is where you should register your own controllers.
        */
        void setup_(int argc, char **argv);

        /**
        * @brief Initializes all the controllers registered in @ref App::setup. Calls @ref rg::Controller::initialize for registered controllers.
        *
        * After this functions finishes all the controllers have been initialized, and they can be now used
        * by calling @ref rg::ControllerManager::get<TController>() or shorter @ref rg::controller.
        */
        void initialize();

        /**
        * @brief Processes all pending events. Calls @ref rg::Controller::poll_events for registered controllers.
        *
        * It handles input events, system events, and any other types of events
        * that have been queued. This is where the platform events are processed.
        */
        void poll_events();

        /**
        * @brief Checks whether the render loop should continue executing. Calls @ref rg::Controller::loop for registered controllers.
        *
        * This is where you should check if an internal or external event happened
        * that stops the main render loop. For example the user presses ESC.
        *
        * @returns true if the main loop should continue, false otherwise.
        */
        bool loop();

        /**
        * @brief Updates the app logic state. Calls @ref rg::Controller::update for registered controllers.
        *
        * This is where all the App state should be updated including handling events
        * registered in @ref App::poll_events, processing physics, world logic etc.
        */
        void update();

        /**
        * @brief Draws the frame. Calls @ref rg::Controller::draw for registered controllers.
        *
        * This is where all the drawing should happen based on the state
        * that the @ref App::update computed.
        */
        void draw();

        /**
        * @brief Terminates the app. Calls @ref rg::Controller::terminate for registered controllers in the **reverse order**.
        *
        * Terminate is called always, regardless of whether the app closes successfully or an error occurs.
        */
        void terminate();

    protected:
        /**
        * @brief Override to define your custom app setup that gets called after engine `setup_`.
        *
        * This function gets called by the `App::setup_` after the engine finishes internal setup.
        */
        virtual void setup();

        /**
        * @brief Called right before the App exits.
        *
        * This where you can do custom operation when all the app state has already been terminated,
        * and all the internal engine systems have been shutdown.
        * This functions gets called just before the return from main.
        *
        * @returns The return value from the int main(...).
        */
        virtual int on_exit() {
            return 0;
        }

        virtual void handle_error(const Error &);

    public:
        virtual ~App() = default;
    };
} // namespace rg

#endif//APP_H

