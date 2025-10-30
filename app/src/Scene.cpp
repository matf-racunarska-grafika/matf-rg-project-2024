//
// Created by matija on 10/30/25.
//

#include "../include/Scene.hpp"

#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <cstdlib>
#include <cmath>

namespace app {
Scene::Scene() {
    init_scene();
}
void Scene::update(float dt, float light_intensity) {
    if (m_swarm_enabled) {
        light_swarm.move_lights(dt);
        light_swarm.set_light_dim(light_intensity);
    }
}
void Scene::dim_lights(float factor) {
    for (auto &l : lights) {
        l.change_brightness(factor);
    }
}
void Scene::draw_static_scene(engine::resources::Shader *s) {
    for (auto &model : models) {
        model.draw(s);
    }
}

void Scene::toggle_swarm() {
    m_swarm_enabled = !m_swarm_enabled;
}


void Scene::init_scene() {
    //lights
    auto l= engine::graphics::Light(
            engine::graphics::LightType::Point,
            glm::vec3(0, 0, 0),
            glm::vec3(1,1,1));
    lights.push_back(l);

    auto l2= engine::graphics::Light(
            engine::graphics::LightType::Spot,
            glm::vec3(0, 0, 0),
            glm::vec3(0,-1,0)
            ,glm::vec3(1,1,1));
    lights.push_back(l2);



    light_swarm = LightSwarm();


    auto floor = engine::core::Controller::get<engine::resources::ResourcesController>()->model("floor");
    MyModel mfloor =MyModel(floor,glm::vec3(0.0f, -0.01f, 0.0f),0.5,0.0f,0.0f,0.0f);
    models.push_back(mfloor);


    auto grass = engine::core::Controller::get<engine::resources::ResourcesController>()->model("gress");
    prepare_grass(-50.0f,50.0f,-50.0f,50.0f,1000);
    MyModel mgrass =MyModel(floor,glm::vec3(0.0f, 0.0f, 0.0f),0.5,0.0f,0.0f,0.0f);
    models.push_back(mgrass);

    //...

}


void Scene::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}
void Scene::draw_lights() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("ligh_shader");
    light_swarm.draw(shader);
}
std::vector<engine::graphics::Light> Scene::get_lights() {
    std::vector<engine::graphics::Light> res(lights);
    res.insert( lights.begin(),light_swarm.get_lights().begin(),light_swarm.get_lights().end());
    return res;
}

void Scene::prepare_grass(float fromx, float tox, float fromy, float toy, uint32_t count=1000) {
    uint32_t num_of_inst=count;
    float radius = 5.0;
    float offset = 2.0f;
    float tilt_angle_bias=15.0f;
    int bush_min=3;
    int bush_max=8;
    int scale_wiggle=15;
    auto grass_model=engine::core::Controller::get<engine::resources::ResourcesController>()->model("grass");
    glm::mat4* trans=new glm::mat4[num_of_inst];
    srand(engine::core::Controller::get<engine::platform::PlatformController>()->frame_time().current);

    int i=0;
        while (i<num_of_inst) {
        int bush_count= bush_min + rand() % (bush_max-bush_min);
        for (uint32_t j=0; j<bush_count&&i<num_of_inst; j++,i++) {

            glm::mat4 model = glm::mat4(1.0f);
            float angle = (float)i / (float)num_of_inst * 360.0f;
            float displacement = (rand() % (int)(2 * offset * 1000)) / 1000.0f - offset;
            float x = sin(angle) * radius + displacement;
            displacement = (rand() % (int)(2 * offset * 1000)) / 1000.0f - offset;
            float y = -std::abs(displacement) * 0.4f;
            displacement = (rand() % (int)(2 * offset * 1000)) / 1000.0f - offset;
            float z = cos(angle) * radius + displacement;

            float scale =1+ (rand() % (2*scale_wiggle)-scale_wiggle) / 100.0f;

            glm::vec3 dir=glm::vec3(0,1,0)+ glm::vec3(tan(tilt_angle_bias)) * glm::normalize(glm::vec3(x, y, z));

            model = glm::translate(model, glm::vec3(x, y, z));
            model = glm::scale(model, glm::vec3(scale));

            float yaw= glm::radians(-180.0f +  (std::rand()*1.0f) / (RAND_MAX*1.0f) *  360.0f);
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f));
            model=model*rotation;

            trans[i]=model;
        }
    }
    grass_model->instantiate(trans,num_of_inst);
    delete[] trans;
}

void Scene::scatter_lights(float fromx, float tox, float fromy, float fromz, float toz, float toy, uint32_t count) {
    glm::vec3 light_col=glm::vec3(0.7f, 1.0f, 0.3f);

    std::srand(static_cast<unsigned int>(engine::core::Controller::get<engine::platform::PlatformController>()->frame_time().current));

    std::vector<engine::graphics::Light> generated;
    generated.reserve(count);

    for (int i = 0; i < count; ++i) {
        const float x = fromx + (std::rand()*1.0f) / (RAND_MAX*1.0f) * (tox - fromx);
        const float y = fromy + (std::rand()*1.0f) / (RAND_MAX*1.0f) * (toy - fromy);
        const float z = fromz + (std::rand()*1.0f) / (RAND_MAX*1.0f) * (toz - fromz);

        engine::graphics::Light light(
            engine::graphics::LightType::Point,
            glm::vec3(x, y, z),
            light_col
        );

        generated.push_back(light);
    }

    light_swarm.set_lights(generated);
}

}// namespace app