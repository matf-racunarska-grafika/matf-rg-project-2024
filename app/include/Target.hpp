//
// Created by boce on 9/14/25.
//

#ifndef TARGET_HPP
#define TARGET_HPP
#include <engine/resources/Model.hpp>
#include <Lights.hpp>


namespace app {

class Target {
public:
    static constexpr float ANGLE_LOWER = -88.0f;
    static constexpr float ANGLE_UPPER = 0.0f;
    static constexpr float ANGLE_SPEED = 75.0f;
    static constexpr float SCALE = 0.11f;

    bool m_active{false};
    glm::vec3 box_min{};
    glm::vec3 box_max{};

    Target(engine::resources::Model *model, const glm::vec3 &position);
    void draw(const engine::resources::Shader *shader, const DirectionalLight &dirlight, const SpotLight &spotlight);

    void put_up(float dt);
    void put_down(float dt);
    void update(float dt);

    void calculate_bounding_box();

    bool check_boundingbox_intersect(const glm::vec3 &raycast_origin, const glm::vec3 &raycast_dir);

private:
    engine::resources::Model *m_model;
    float m_angle{ANGLE_LOWER};
    float m_scale{SCALE};
    glm::vec3 m_position{};
};
}// app

#endif //TARGET_HPP
