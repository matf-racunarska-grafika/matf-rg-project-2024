
//

#ifndef MATF_RG_PROJECT_ENGINECONTROLLERSSENTINEL_HPP
#define MATF_RG_PROJECT_ENGINECONTROLLERSSENTINEL_HPP
#include <engine/controller/Controller.hpp>
#include <memory>
namespace rg {

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

    class EngineControllersSentinel : public rg::Controller {
    public:
        static std::unique_ptr<EngineControllersSentinel> create() {
            return std::make_unique<EngineControllersSentinel>();
        }

        std::string_view name() const override {
            return "EngineControllersSentinel";
        }
    };
}// namespace rg
#endif//MATF_RG_PROJECT_ENGINECONTROLLERSSENTINEL_HPP
