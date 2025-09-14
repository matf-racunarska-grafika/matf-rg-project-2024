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
    bool m_active{false};
    glm::vec3 box_min{};
    glm::vec3 box_max{};

    Target(engine::resources::Model *model, const glm::vec3 &position);
    void draw(const engine::resources::Shader *shader, const DirectionalLight &dirlight, const SpotLight &spotlight);

private:
    engine::resources::Model *m_model;
    float m_angle{-88.0f};
    float m_scale{0.11f};
    glm::vec3 m_position{};


};
}// app

#endif //TARGET_HPP
