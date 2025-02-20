#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace app {
    class MainController final : public engine::core::Controller {
        void initialize() override;

        bool loop() override;

        void update_camera();

        void update() override;

        void draw() override;

        void draw_skull();

        void begin_draw() override;

        void end_draw() override;
    public:
        std::string_view name() const override { return "app::MainController"; }
    };
}
#endif //MAINCONTROLLER_HPP
