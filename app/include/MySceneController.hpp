//
// Created by matija on 10/12/25.
//

#ifndef MATF_RG_PROJECT_MYSCENECONTROLLER_HPP
#define MATF_RG_PROJECT_MYSCENECONTROLLER_HPP
#include <LightSwarm.hpp>
#include <Scene.hpp>
#include <Timer.hpp>
#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <glm/fwd.hpp>

namespace app {
class ScenePlatformEventObserver final : public engine::platform::PlatformEventObserver {
public:
    void on_key(engine::platform::Key key) override;
    void on_window_resize(int width, int height) override;
};

class MySceneController final : public engine::core::Controller {
public:
    std::string_view name() const override {
        return "test::app::MyMainController";
    }
    void set_dim(float brightness);
    void start_animation();
    void set_window_size(int width, int height);

    Scene* scene() { return &m_scene; }

    void set_threshold(float brightness);

    void set_bloom_intensity(float brightness);

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;



    void end_draw() override;

    void update_camera();


    bool m_cursor_enabled{true};

    Scene m_scene;
    Timer m_delay_timer;
    Timer m_duration_timer;

    float m_duration_amount=30.0f;
    float m_delay_amount=3.0f;
};
} // app

#endif //MATF_RG_PROJECT_MYSCENECONTROLLER_HPP