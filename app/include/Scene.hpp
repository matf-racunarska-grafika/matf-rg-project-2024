//
// Created by matija on 10/30/25.
//

#ifndef MATF_RG_PROJECT_SCENE_HPP
#define MATF_RG_PROJECT_SCENE_HPP
#include <../../engine/include/engine/resources/Light.hpp>
#include <LightSwarm.hpp>
#include <MyModel.hpp>
#include <engine/graphics/BloomFilter.hpp>
#include <engine/graphics/DeferredFilter.hpp>
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

    void set_width_height(int width, int height);

    std::vector<engine::graphics::Light> get_lights();
    void draw();
    void init_scene();

private:
    std::vector<engine::graphics::Light> lights;
    std::vector<MyModel> models;
    bool m_swarm_enabled = false;

    app::LightSwarm* light_swarm=nullptr;
    engine::graphics::DeferredFilter*  m_deferred_filter=nullptr;
    engine::graphics::BloomFilter*  m_bloom_filter=nullptr;




    void prepare_grass(float fromx, float tox, float fromy, float toy, uint32_t count);
    void scatter_lights(float fromx, float tox, float fromy, float toy, float fromz, float toz,  uint32_t count);
};

}// namespace app

#endif//MATF_RG_PROJECT_SCENE_HPP
