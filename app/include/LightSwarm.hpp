//
// Created by matija on 10/30/25.
//

#ifndef MATF_RG_PROJECT_LIGHTSWARM_HPP
#define MATF_RG_PROJECT_LIGHTSWARM_HPP
#include <../../engine/include/engine/resources/Light.hpp>
#include <engine/resources/Shader.hpp>
#include <vector>


namespace app {
class LightSwarm {
public:

    LightSwarm();
    ~LightSwarm();

    void add_light(const engine::graphics::Light &light);
    const std::vector<engine::graphics::Light> &get_lights() const;
    void set_lights(const std::vector<engine::graphics::Light> &lights);
    int get_light_count() const;

    //draw light cubes at light positions
    void draw(engine::resources::Shader* shader );
    //move lights in a swarm pattern, alongside vector in random direction
    void move_lights(float dt);

    void set_light_dim(float dim);

private:
    unsigned int light_vao;
    std::vector<engine::graphics::Light> lights;
    float m_speed{1.0f};
    float m_size{0.1f};
    float m_light_dim=1.0f;
};
}

#endif //MATF_RG_PROJECT_LIGHTSWARM_HPP