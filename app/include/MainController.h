//
// Created by aleksa on 1.6.25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include<glm/vec3.hpp>
#include <engine/resources/ResourcesController.hpp>


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

    void draw_sun();
    void draw_asteroids();

    void set_up_shader_uniforms(engine::resources::Shader *shader, bool dirL, bool pointL);

    void update_scene();
    void register_button();

    glm::vec3 sun_rot{1.0f, 0.0f, 0.0f};
    float sun_scale{1.81f};
    float sun_angle{45.0f};


    void draw_ufo();
    glm::vec3 ufo_pos{1.0f, 1.0f, 1.0f};
    glm::vec3 ufo_pos_start{1.0f, 1.0f, 1.0f};
    glm::vec3 ufo_rot{1.0f, 0.0f, 0.0f};
    float ufo_scale{0.43f};
    float ufo_angle{-41.0f};

    float sun_lights_up_timer{0.0f};
    float sun_starts_expanding_timer{0.0f};
    bool sequence_active = false;
    bool ufo_visible = true;
    bool not_blocked = false;
    float timer_for_scaling{0.0f};

    float emission_strength_start = 1.0f;

    float sun_scale_start{1.81f};

    glm::vec3 asteroid_pos{-18.0f, -33.0f, -36.0f};
    glm::vec3 asteroid_rot{1.0f, 0.0f, 0.0f};
    float asteroid_scale{0.5f};
    float asteroid_angle{78.0f};

    bool draw_gui{false};
    bool cursor_enabled{true};

public:
    std::string_view name() const override { return "App::MainController"; }

    float emission_strength = 1.0f;
    glm::vec3 emissive_color = glm::vec3(0.15f, 0.05f, 0.0f);


    glm::vec3 dirLightDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 dirLightColorAmbient = glm::vec3(0.03f, 0.03f, 0.03f);
    glm::vec3 dirLightColorDiffuse = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 dirLightColorSpecular = glm::vec3(0.2f, 0.2f, 0.02f);



    glm::vec3 sun_pos{5.0f, 1.0f, -80.0f};
    glm::vec3 pointLightColorDiffuse = glm::vec3(1.0f, 0.6f, 0.2f);
    glm::vec3 pointLightColorSpecular = glm::vec3(1.0f, 0.6f, 0.2f);
    glm::vec3 pointLightColorAmbient = glm::vec3(0.4f, 0.4f, 0.4f);
    float pointLightConstant = 1.0f;
    float pointLightLinear = 0.00001f;
    float pointLightQuadratic = 0.00005f;
};

}// app

#endif //MAINCONTROLLER_H
