//
// Created by filipn on 9/1/25.
//

#ifndef MAINCONTROLER_HPP
#define MAINCONTROLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/resources/Shader.hpp>
#include <glm/vec3.hpp>

namespace app {
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

    void draw_moon();

    void draw_sun();

    void set_point_light(engine::resources::Shader *shader, const PointLight &pointLight, const std::string &lightName);

    void set_spot_light(engine::resources::Shader *shader, int i, glm::vec3 lightPosition);

    void draw_island(engine::resources::Shader *shader, bool shadowPass = false);

    void update_camera();

    void update() override;

    void begin_draw() override;

    void draw_skybox();

    void draw_tree(engine::resources::Shader *shader);

    void draw_bench(engine::resources::Shader *shader);

    void draw_lamp(engine::resources::Shader *shader, bool shadowPass = false);

    void draw_bush(engine::resources::Shader *shader);

    void draw_path(engine::resources::Shader *shader);

    void draw_quad();

    void draw() override;

    void end_draw() override;

    PointLight moonlight;
    PointLight sunlight;

public:
    std::string_view name() const override { return "MainController"; }
};

}
#endif //MAINCONTROLER_HPP
