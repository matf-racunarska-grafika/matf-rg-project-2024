#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <spdlog/spdlog.h>
#include <engine/graphics/GraphicsController.hpp>

namespace app {

    extern float test_x, test_y, test_z;
    extern float test_rotation_x, test_rotation_y, test_rotation_z;
    extern float test_scale;
    extern float wind_speed;


class MainController : public engine::core::Controller {

    void initialize() override;
    bool loop() override;
    void begin_draw() override;
    void end_draw() override;
    void draw() override;

    void test();

    void draw_skybox();
    void draw_terrain();
    void draw_water();
    void draw_forest();

    void draw_campfire();
    void draw_logs();
    void draw_tents();
    void draw_old_tree();
    void draw_bushes();
    void draw_white_flowers();
    void draw_red_flowers();
    void draw_flowers();
    void draw_path();
    void draw_mushrooms();
    void draw_lighthouse();
    void draw_stones();
    void draw_fire();

    void update() override;
    void update_camera();

    bool is_day = true;
    void set_common_shader_variables(engine::resources::Shader* shader, engine::graphics::Camera* camera, engine::graphics::GraphicsController* graphics) {
        auto lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);
        auto ambient = is_day ? glm::vec3(0.2f) : glm::vec3(0.1f);
        auto diffuse = is_day ? glm::vec3(0.5f) : glm::vec3(0.3f);
        auto specular = is_day ? glm::vec3(0.1) : glm::vec3(0.05f);
        float shininess = is_day ? 1024.0f : 2048.0f;
        auto lightColor = is_day ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(1.0f, 0.7f, 0.1f);
        shader->use();
        shader->set_vec3("light.position", lightPos);
        shader->set_vec3("light.ambient", ambient);
        shader->set_vec3("light.diffuse", diffuse);
        shader->set_vec3("light.specular", specular);
        shader->set_vec3("lightColor", lightColor);
        shader->set_float("material.shininess", shininess);
        shader->set_vec3("viewPos", camera->Position);
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", camera->view_matrix());
    }

public:

    std::string_view name() const override {
        return "app::MainController";
    }
};
}

#endif //MAINCONTROLLER_HPP
