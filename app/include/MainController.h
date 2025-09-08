//
// Created by aleksa on 1.6.25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include<glm/vec3.hpp>

namespace app {
class MainPlatformEventObserver final : public engine::platform::PlatformEventObserver {
public:
    void on_key(engine::platform::Key key) override;

    void on_mouse_move(engine::platform::MousePosition position) override;

    void on_scroll(engine::platform::MousePosition position);
};

class MainController final : public engine::core::Controller {
private:
    void initialize() override;

    void draw_skybox();

    bool loop() override;

    void draw() override;

    void begin_draw() override;

    void end_draw() override;

    void update_camera();

    bool draw_gui{false};
    bool cursor_enabled{true};

public:
    std::string_view name() const override { return "App::MainController"; }
};

}// app

#endif //MAINCONTROLLER_H
