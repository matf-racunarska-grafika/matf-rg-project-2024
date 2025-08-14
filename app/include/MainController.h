//
// Created by aleksa on 1.6.25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>

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

    void draw_goalkeeper();

    void draw_red_players();

    void draw_blue_players();

    void draw_reflectors();

    void draw_grass();

    void draw_goal();

    void draw_ball();

    void update() override;

    void update_camera();

    bool draw_gui{false};
    bool cursor_enabled{true};

public:
    std::string_view name() const override { return "App::MainController"; }

    float gol_skalirano{1.0f};

    float ugao_gol{0.26f};

    float gol_x{0.15f};
    float gol_y{1.005f};
    float gol_z{-5.175f};

    float gol_xv{1.0f};
    float gol_yv{0.0f};
    float gol_zv{0.0f};

    float trava_skalirano{1.3f};

    float ugao_trava{0.0f};

    float trava_x{1.0f};
    float trava_y{1.0f};
    float trava_z{1.0f};

    float trava_xv{1.0f};
    float trava_yv{0.0f};
    float trava_zv{0.0f};

    float crveni_skalirano{0.17f};

    float ugao_crveni{0.0f};

    float crveni_x{1.95f};
    float crveni_y{1.865f};
    float crveni_z{10.905f};

    float crveni_xv{1.0f};
    float crveni_yv{0.0f};
    float crveni_zv{0.0f};

    float plavi_skalirano{0.175f};

    float ugao_plavi{180.0f};

    float plavi_x{1.0f};
    float plavi_y{1.865f};
    float plavi_z{6.135f};

    float plavi_xv{0.0f};
    float plavi_yv{1.0f};
    float plavi_zv{0.0f};

    float lopta_skalirano{0.01f};

    float lopta_x{1.965f};
    float lopta_y{1.105f};
    float lopta_z{10.645f};

    float ref_skalirano{0.01f};

    float ugao_ref1{-99.98f};

    float ref_x{12.0f};
    float ref_y{1.005f};
    float ref_z{-8.87f};

    float ref_xv{0.96f};
    float ref_yv{-0.465f};
    float ref_zv{-0.44f};

    float ref2_skalirano{0.01f};

    float ugao_ref2{144.0f};

    float ref2_x{-11.85f};
    float ref2_y{1.005f};
    float ref2_z{-9.095f};

    float ref2_xv{-0.62f};
    float ref2_yv{1.415f};
    float ref2_zv{1.4f};

    float golman_skalirano{0.175f};

    float ugao_golman{180.0f};

    float golman_x{-1.045f};
    float golman_y{1.865f};
    float golman_z{-4.25f};

    float golman_xv{0.0f};
    float golman_yv{1.0f};
    float golman_zv{0.0f};

};

}// app

#endif //MAINCONTROLLER_H
