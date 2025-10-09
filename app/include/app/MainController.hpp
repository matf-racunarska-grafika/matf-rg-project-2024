
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
        return "main::app::MainController";
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

    void draw_busStop();

    void draw_jellyFish();

    void update_camera();

    std::vector<glm::vec3> m_jellyFishPositions;

    bool m_isDay{true};
    bool m_draw_gui{false};
    bool m_cursor_enabled{true};
};
}
#endif //MAINCONTROLLER_HPP
