//
// Created by matija on 10/30/25.
//

#ifndef MATF_RG_PROJECT_SCENE_HPP
#define MATF_RG_PROJECT_SCENE_HPP
#include <../../engine/include/engine/resources/Light.hpp>
#include <LightSwarm.hpp>
#include <MyModel.hpp>
#include <engine/resources/Model.hpp>
#include <vector>

namespace app {

class Scene {

public:
    Scene();
    void update(float dt,float light_intensity);
    void dim_lights(float factor);

    void draw_static_scene(engine::resources::Shader * s);
    void toggle_swarm();

    void draw_skybox();
    void draw_lights();
    std::vector<engine::graphics::Light> get_lights();

private:
    std::vector<engine::graphics::Light> lights;
    app::LightSwarm light_swarm;
    std::vector<MyModel> models;
    bool m_swarm_enabled = false;

    void init_scene();

    void prepare_grass(float fromx, float tox, float fromy, float toy, uint32_t count);
    void scatter_lights(float fromx, float tox, float fromy, float toy, float fromz, float toz,  uint32_t count);
};

}// namespace app

#endif//MATF_RG_PROJECT_SCENE_HPP
