//
// Created by matfrg on 7/14/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

namespace app {

struct PointLight {
    glm::vec3 ambient = glm::vec3(0.7f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
    glm::vec3 intensity = glm::vec3(0.6f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    float linear = 0.004f;
    float quadratic = 0.0001f;
    float shininess = 32.0f;
};

struct SpotLight {
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
    float cutOff = cos(glm::radians(20.0f));
    float outer_cutOff = cos(glm::radians(30.0f));

    float linear = 0.004f;
    float quadratic = 0.0001f;
    float shininess = 32.0f;
};

class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void begin_draw() override;

    void draw_floor();

    void draw_lamp();

    void draw_lantern();

    void draw_dog();

    void draw_graves();

    void draw_car();

    void update() override;

    void update_camera();

    void end_draw() override;

    void draw() override;

public:
    std::string_view name() const override { return "app::MainController"; }
    PointLight point_light;
    SpotLight spot_light;
    bool spotlight_switch = true;
};

}// app

#endif //MAINCONTROLLER_HPP
