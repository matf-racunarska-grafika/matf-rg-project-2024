/**
 * @file Controller.hpp
 */

#ifndef MATF_RG_PROJECT_CONTROLLER_HPP
#define MATF_RG_PROJECT_CONTROLLER_HPP

#include <string_view>
#include <vector>
#include <memory>

namespace rg {
    /**
    * @class Controller
    * @breif Controllers are a hook into the Engines `main loop` execution.
    * By overriding member virtual functions of this class the user can
    * execute custom code during each of the `main loop` phases.
    *
    * @usage
    * \code
    * class LoggingController : public rg::Controller {
    * public:
    *
    *   void initialize() override { spdlog::log("Logging::initialize"); }
    *   void terminate() override { spdlog::log("Logging::terminate"); }
    * };
    * \endcode
    * To have the engine execute the code from the `LoggingController` we must first register it:
    * \code
    * rg::ControllerManager::instance()->register_controller<LoggingController>();
    * \endcode
    *
    */
    class Controller {
        friend class ControllerManager;

    public:
        /**
        * Returns the controller class name; used for logging.
        * @return Controller name
        */
        virtual std::string_view name() const = 0;

        virtual ~Controller() = default;

        /**
        * Orders the controller `next` to be after `this` controller.
        * All the virtual member methods of the controller `this` will always execute
        * before the member functions of the controller `next`.
        */
        void before(Controller *next) {
            this->m_next.push_back(next);
        }

        /**
        * Orders the controller `this` to be after `prev` controller.
        * All the virtual member methods of the controller `prev` will always execute
        * before the member functions of the controller `this`.
        */
        void after(Controller *prev) {
            prev->before(this);
        }

        /**
         * Returns the controllers that have a direct dependency to `this` controller.
         * Example:
         * ```c++
         * ```
        * @returns Controllers executing after `this`
        */
        const std::vector<Controller *> &next() const {
            return m_next;
        }

        /**
        * \breif Used by the @ref ControllerManager to ensure that the controller was initialized
        * before being used.
        * @return true if @ref Controller::initialize() has been called
        */
        bool is_initialized() const {
            return m_initialized;
        }

    private:
        void mark_initialized() {
            m_initialized = true;
        }

        virtual void initialize() {
        }

        virtual bool loop() {
            return true;
        }

        virtual void poll_events() {
        }

        virtual void begin_frame() {
        }

        virtual void update() {
        }

        virtual void draw() {
        }

        virtual void end_frame() {
        }

        virtual void terminate() {
        }

        /* List of controller that are dependent on this controller */
        std::vector<Controller *> m_next{};
        bool m_initialized{false};
    };

    /**
     * @class EngineControllersSentinel
     * @brief This controller does nothing. It servers as a sentinel
     * controller between user controllers and engine controllers
     * if the user wants his custom controller to execute after
     * all the engine controllers.
     *
     *
     * @code
     * auto user_controller = ...;
     * user_controller->after(rg::ControllerManager::instance()->get<EngineControllersSentinel>());
     * @endcode
     * This will make user_controller execute after all the engine controllers.
     */
    class EngineControllersSentinel : public Controller {
    public:
        std::string_view name() const override {
            return "EngineControllersSentinel";
        }
    };
} // namespace rg

#endif//MATF_RG_PROJECT_CONTROLLER_HPP
