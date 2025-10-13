//
// Created by matija on 10/12/25.
//

#ifndef MATF_RG_PROJECT_MYSCENECONTROLLER_HPP
#define MATF_RG_PROJECT_MYSCENECONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <glm/fwd.hpp>

namespace app {
class ScenePlatformEventObserver final : public engine::platform::PlatformEventObserver {
public:
    void on_key(engine::platform::Key key) override;
    void on_mouse_move(engine::platform::MousePosition position) override;
};

class MySceneController final : public engine::core::Controller {
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

    void prepare_grass(float fromx, float tox, float fromy, float toy, uint32_t count);

    void draw_grass();

    void draw_floor();



    //#TODO add state..
    //bool m_draw_gui{false};
    //bool m_cursor_enabled{true};

};
} // app

#endif //MATF_RG_PROJECT_MYSCENECONTROLLER_HPP