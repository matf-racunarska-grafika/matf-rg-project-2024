//
// Created by spaske on 12/7/24.
//

#ifndef ENGINESENTINELCONTROLLER_HPP
#define ENGINESENTINELCONTROLLER_HPP
#include <engine/controller/Controller.hpp>

namespace engine::controller {
    /**
     * @class EngineSentinelController
     * @brief This controller does nothing. It servers as a sentinel
     * controller between user controllers and engine controllers
     * if the user wants his custom controller to execute after
     * all the engine controllers.
     *
     *
     * This will make user_controller execute after all the engine controllers.
     * @code
     * auto user_controller = ...;
     * user_controller->after(engine::controller::get<EngineSentinelController>());
     * @endcode
     */
    class EngineSentinelController final : public Controller {
    public:
        std::string_view name() const override {
            return "EngineSentinelController";
        }
    };
}
#endif //ENGINESENTINELCONTROLLER_HPP
