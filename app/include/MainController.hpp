//
// Created by zesla on 10/9/25.
//
#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <glm/vec3.hpp>

namespace app{
class MainController: public engine::core::Controller {
public:
    void initialize() override;
    bool loop() override;
    void begin_draw() override;
    void draw() override;
    void end_draw() override;
    void update() override;
    std::string_view name() const override {
        return "app::MainController";
    }

private:
    void draw_bed();
    void draw_cloud();
    void draw_table();
    void draw_lamp();
    void draw_skybox();
    void update_camera();
    void update_lamp_flicker();
    void setup_lighting(engine::resources::Shader* shader);
    void setup_cloud_lighting();
    void setup_clouds();
    void update_clouds();

    glm::vec3 m_cloud_transl_factor = glm::vec3(0.0f, 5.0f, 0.0f);
    glm::vec3 m_cloud_scale_factor = glm::vec3(1.0f);

    bool m_lamp_is_on = true;
    glm::vec3 m_lamp_position = glm::vec3(7.0f, 1.2f, 7.0f);
    bool m_start_flicker_sequence = false;
    bool m_lamp_is_flickering = false;
    float m_flicker_delay_timer = 0.0f;
    float m_flicker_timer = 0.0f;
    float m_flicker_interval = 0.0f;

    float m_cloud_orbit_time = 0.2f;
    float m_orbit_radius = 20.0f;
    int m_num_clouds = 10;

    bool m_morbin_time = false;

};
}
#endif//MATF_RG_PROJECT_MAINCONTROLLER_HPP
