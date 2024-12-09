//
// Created by spaske on 12/9/24.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

namespace app {
    class MainPlatformEventObserver final : public engine::platform::PlatformEventObserver {
    public:
        void on_keyboard(engine::platform::Key key) override;
    };

    class MainController final : public engine::controller::Controller {
        void initialize() override;

        bool loop() override;

        void poll_events() override;

        void update() override;

        void begin_draw() override;

        void draw() override;

        void end_draw() override;

        void draw_skybox();

        void draw_backpack();

        void update_camera();

        float m_backpack_scale{1.0f};
        bool m_draw_gui{false};
        bool m_cursor_enabled{true};
    };
}
#endif //MAINCONTROLLER_HPP
