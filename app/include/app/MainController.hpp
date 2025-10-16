
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

    glm::vec3 m_busStartPos{-2.0f, -0.47f, -3.0f};
    float m_busOffset = 0.0f;
    float m_busDistance = 10.0f;
    float m_busSpeed = 0.05f;
    bool m_busMoving = false;

    std::array<std::string, 6> m_daySkyboxTextures;
    std::array<std::string, 6> m_nightSkyboxTextures;

    void reload_skybox_textures(); // funkcija koja menja teksture u letu


};
}
#endif //MAINCONTROLLER_HPP
