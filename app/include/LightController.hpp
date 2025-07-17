//
// Created by cvnpko on 7/17/25.
//

#ifndef LIGHTCONTROLLER_HPP
#define LIGHTCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>
#include <MainEventController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/core/Engine.hpp>

namespace app {

class LightController final : public engine::core::Controller {
public:
    std::string_view name() const override { return "app::LightController"; }

    void setLight(engine::resources::Shader *shader);

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void draw_moon();

    void draw_sun();

    void end_draw() override;

    bool m_spotlight_enabled{false};

    glm::vec3 m_sun_light{glm::vec3(255.0f, 255.0f, 224.0f) / 255.0f};
    float m_sun_light_coeff{0.9f};

    glm::vec3 m_moon_light{glm::vec3(247.0f, 234.0f, 198.0f) / 255.0f};
    float m_moon_light_coeff{0.2f};

    float m_ambient_coeff{0.1f};
    float angle{0.0f};

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

}

#endif //LIGHTCONTROLLER_HPP
