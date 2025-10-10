//
// Created by nikola on 10/8/25.
//

#ifndef MATF_RG_PROJECT_MYCONTROLLER_H
#define MATF_RG_PROJECT_MYCONTROLLER_H
#include "engine/core/Controller.hpp"
#include <vector>

#include <Drawable.hpp>
#include <glm/vec3.hpp>

namespace app {
class Drawable;

struct PointLight {
    glm::vec3 ambient = glm::vec3(1,0,0);
    glm::vec3 diffuse = glm::vec3(0.6f);
    glm::vec3 specular = glm::vec3(0.4f);
    glm::vec3 position = glm::vec3(-7.0f, 0.0f, -9.0f);
    float linear = 0.004f;
    float quadratic = 0.0001f;
    float shininess = 32.0f;
    glm::vec3 intensity = glm::vec3(0.1f);
};

struct DirectionalLight {
    glm::vec3 direction = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 ambient = glm::vec3(0.3f);
    glm::vec3 diffuse = glm::vec3(0.7f);
    glm::vec3 specular = glm::vec3(0.5f);
    float shininess = 8.0f;
    glm::vec3 intensity = glm::vec3(1.0f);
};

class MyController : public engine::core::Controller {
    std::vector<Drawable*> m_drawables;

    void initialize() override;
    bool loop() override;

    void update_camera();
    void update() override;

    void begin_draw() override;
    void draw_skybox();
    void draw() override;
    void end_draw() override;

public:
    PointLight point_light;
    DirectionalLight directional_light;
    ~MyController();
    std::string_view name() const override {return "app::MyController";}

    void addDrawable(Drawable* drawable);
};

}// namespace app

#endif//MATF_RG_PROJECT_MYCONTROLLER_H
