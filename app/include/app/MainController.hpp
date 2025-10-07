
#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

namespace engine::app {
class MainPlatformEventObserver final : public engine::platform::PlatformEventObserver {
public:
    void on_key(engine::platform::Key key) override;

    void on_mouse_move(engine::platform::MousePosition position) override;
};

class MainController final : public engine::core::Controller {
public:
    std::string_view name() const override {
        return "test::app::MainController";
    }

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void draw_skybox();

    void draw_ground();

    void draw_spongebob();

    void draw_SBHouse();

    void draw_gary();

    void draw_bus();

    void update_camera();

    float m_spongebob_scale{1.0f};
    bool m_draw_gui{false};
    bool m_cursor_enabled{true};
};
}
#endif //MAINCONTROLLER_HPP
