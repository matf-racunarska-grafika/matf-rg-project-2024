//
// Created by matija on 10/30/25.
//

#ifndef MATF_RG_PROJECT_LIGHTSWARM_HPP
#define MATF_RG_PROJECT_LIGHTSWARM_HPP
#include <engine/resources/Light.hpp>
#include <engine/resources/Shader.hpp>
#include <vector>


namespace app {
class LightSwarm {
public:
    LightSwarm();
    ~LightSwarm();
    void add_light(const engine::graphics::Light &light);
    const std::vector<engine::graphics::Light> &lights() const;
    void set_lights(const std::vector<engine::graphics::Light> &ls);
    int light_count() const;
    void draw(engine::resources::Shader* shader );
    void move_lights(float dt);
    void set_light_dim(float dim);
private:
    unsigned int m_light_vao;
    std::vector<engine::graphics::Light> m_lights;
    float m_speed{0.5f};
    float m_size{0.008f};
    float m_light_dim=1;
};
}

#endif //MATF_RG_PROJECT_LIGHTSWARM_HPP