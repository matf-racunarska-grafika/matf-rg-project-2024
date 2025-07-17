//
// Created by cvnpko on 7/14/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <memory>
#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/core/Engine.hpp>

namespace app {

class MainController final : public engine::core::Controller {
public:
    std::string_view name() const override { return "app::MainController"; }

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw_skybox();

    void draw_dunes();

    void draw_pyramids();

    void draw_sphinx();

    void draw_moon();

    void draw_sun();

    void draw_camels();

    void draw() override;

    void end_draw() override;

    void update_camera();

    bool m_cursor_enabled{true};
    glm::vec3 m_sun_light{glm::vec3(255.0f, 255.0f, 224.0f) / 255.0f};
    float m_sun_light_coeff{0.8f};
    glm::vec3 m_moon_light{glm::vec3(247.0f, 234.0f, 198.0f) / 255.0f};
    float m_moon_light_coeff{0.2f};
    float m_ambient_coeff{0.1f};

    struct PointLight {
        glm::vec3 position;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float constant;
        float linear;
        float quadratic;

    } m_point_light_sun, m_point_light_moon;
};

}// app

#endif //MAINCONTROLLER_HPP
