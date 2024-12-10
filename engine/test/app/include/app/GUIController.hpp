
#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP
#include <engine/Engine.hpp>

namespace engine::test::app {
    class GUIController final : public engine::controller::Controller {
        void initialize() override;

        void poll_events() override;

        void draw() override;
    };
}
#endif //GUICONTROLLER_HPP
