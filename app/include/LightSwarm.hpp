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

    void draw(engine::resources::Shader* shader );
    void move_lights(float dt);

    void set_light_dim(float dim);

private:
    unsigned int light_vao;
    std::vector<engine::graphics::Light> lights;
    float m_speed{1.2f};
    float m_size{0.02f};
    float m_light_dim=0.5f;
};
}

#endif //MATF_RG_PROJECT_LIGHTSWARM_HPP