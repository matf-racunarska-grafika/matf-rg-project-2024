#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace app {
    class MainController final : public engine::core::Controller {
        void initialize() override;

        bool loop() override;
    };
}
#endif //MAINCONTROLLER_HPP
