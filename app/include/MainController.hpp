//
// Created by ana on 5/13/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/vec3.hpp>

namespace app {

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void draw_car();

    void draw_traffic_light();

    void update_camera();

    void update_lights_go();

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void turn_off(PointLight &light);

    void turn_on(PointLight &light, int color);

public:
    std::string_view name() const override { return "app::MainController"; }
    DirLight directionalLight;
    PointLight redPointLight;
    PointLight yellowPointLight;
    PointLight greenPointLight;

private:
    bool transition_started = false;
    float time_since_transition = 0.0f;
};

}// app

#endif //MAINCONTROLLER_HPP
