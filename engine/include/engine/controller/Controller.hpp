
//

#ifndef MATF_RG_PROJECT_CONTROLLER_HPP
#define MATF_RG_PROJECT_CONTROLLER_HPP

#include <string_view>
#include <vector>
#include <memory>

namespace rg {
    class Controller {
        friend class ControllerManager;

    public:
        virtual std::string_view name() const = 0;

        virtual ~Controller() = default;

        void before(Controller *next) {
            this->m_next.push_back(next);
        }

        void after(Controller *prev) {
            prev->before(this);
        }

        const std::vector<Controller *> &next() const {
            return m_next;
        }

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
     * This controller does nothing. It servers as a sentinel
     * controller between user controllers and engine controllers
     * if the user wants his custom controller to execute after
     * all the engine controllers.
     *
     * Example usage:
     * auto user_controller = ...;
     * user_controller->after(rg::ControllerManager::instance()->get<EngineControllersSentinel>());
     * This will make user_controller execute after all the engine controllers.
     */

    class EngineControllersSentinel : public Controller {
    public:
        static std::unique_ptr<EngineControllersSentinel> create() {
            return std::make_unique<EngineControllersSentinel>();
        }

        std::string_view name() const override {
            return "EngineControllersSentinel";
        }
    };
} // namespace rg

#endif//MATF_RG_PROJECT_CONTROLLER_HPP
