//
// Created by matija on 10/30/25.
//

#ifndef MATF_RG_PROJECT_SCENE_HPP
#define MATF_RG_PROJECT_SCENE_HPP
#include <engine/resources/Light.hpp>
#include <LightSwarm.hpp>
#include <MyModel.hpp>
#include <engine/graphics/BloomFilter.hpp>
#include <engine/graphics/DeferredFilter.hpp>
#include <engine/resources/Model.hpp>
#include <vector>
#include <engine/platform/PlatformController.hpp>

namespace app {

class Scene {

public:
    Scene();
    void update(engine::platform::FrameTime);
    void dim_lights(float factor);

    void draw_static_scene(engine::resources::Shader * s);
    void toggle_swarm();

    void draw_skybox();
    void draw_lights();

    void set_width_height(int width, int height);

    std::vector<engine::graphics::Light> get_lights();

    void draw_instanced();

    void draw();
    void init_scene();

    void draw_bloom();

    void set_bloom_enabled(bool enabled) { m_bloom_enabled = enabled; }
    bool bloom_enabled() const { return m_bloom_enabled; }

    bool swarm_enabled();

    void set_swarm_enabled(bool enabled);

    void set_threshold(float threshold);

    void set_bloom_intensity(float brightness);

    void make_instances(engine::resources::Model *model, float fromx, float tox, float fromz, float toz, float fromy, float toy, uint32_t count);

private:
    std::vector<engine::graphics::Light> m_lights;
    std::vector<MyModel> m_models;
    bool m_swarm_enabled = false;

    app::LightSwarm* m_light_swarm=nullptr;
    app::LightSwarm* m_light_swarm2=nullptr;
    engine::graphics::DeferredFilter*  m_deferred_filter=nullptr;
    engine::graphics::BloomFilter*  m_bloom_filter=nullptr;
    bool m_bloom_enabled = true;

    void prepare_grass(float fromx, float tox, float fromy, float toy, uint32_t count);
    std::vector<engine::graphics::Light>  scatter_lights(float fromx, float tox, float fromy, float toy, float fromz, float toz,  uint32_t count);
};

}// namespace app

#endif//MATF_RG_PROJECT_SCENE_HPP
