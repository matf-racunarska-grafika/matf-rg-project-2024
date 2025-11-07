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
#include <iostream>

namespace app {
Scene::Scene() {
}
void Scene::update(engine::platform::FrameTime time) {
    if (m_swarm_enabled) {
        light_swarm->move_lights(time.dt);
        light_swarm2->move_lights(time.dt);
        float light_intensity = std::max(std::pow(std::sin(time.current * 1.0f)-0.1f, 3.0f), 0.0f);
        float light_intensity2 = std::max(std::pow(std::sin(time.current * 1.0f+1.2f)-0.1f, 3.0f), 0.0f);
        light_swarm->set_light_dim(light_intensity);
        light_swarm2->set_light_dim(light_intensity2);
    }
}
void Scene::dim_lights(float factor) {
    for (auto &l : lights) {
        l.set_brightness(factor);
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
    if (m_deferred_filter==nullptr) {
        m_deferred_filter=new engine::graphics::DeferredFilter();
        auto g=engine::core::Controller::get<engine::graphics::GraphicsController>();
        m_deferred_filter->initilizeBuffers(static_cast<unsigned int>(g->perspective_params().Width),g->perspective_params().Height);
    }
    if (m_bloom_filter==nullptr) {
        m_bloom_filter=new engine::graphics::BloomFilter(0.7,0.7,5);
        auto g=engine::core::Controller::get<engine::graphics::GraphicsController>();
        m_bloom_filter->initilizeBuffers(static_cast<unsigned int>(g->perspective_params().Width),g->perspective_params().Height);
    }
    if (light_swarm==nullptr) {
        light_swarm=new LightSwarm();
    }
    if (light_swarm2==nullptr) {
        light_swarm2=new LightSwarm();
    }


    auto l2= engine::graphics::Light(
            engine::graphics::LightType::Spot,
            glm::vec3(0, 2, 0),
            glm::vec3(0.6,0.6,0.6)
            ,glm::vec3(0,1,0));

    l2.set_cutoff(12,17);
    lights.push_back(l2);

    std::vector<engine::graphics::Light> generated= scatter_lights(-3,3,0,2,-3,3,70);
    light_swarm->set_lights(generated);
    std::vector<engine::graphics::Light> generated2= scatter_lights(-3,3,0,2,-3,3,40);
    light_swarm2->set_lights(generated2);


    auto floor = engine::core::Controller::get<engine::resources::ResourcesController>()->model("floor");
    MyModel mfloor =MyModel(floor,glm::vec3(-0.5f, -0.28, -0.5f),0.035,0.0f,-90.0f,0.0f);
    models.push_back(mfloor);

    auto statue = engine::core::Controller::get<engine::resources::ResourcesController>()->model("sculpture");
    statue->setShininess(32);
    MyModel mstatue =MyModel(statue,glm::vec3(0.0f, -0.05, 0.0f),0.00007,-90.0f,0.0f,0.0f);
    models.push_back(mstatue);

    auto tree1 = engine::core::Controller::get<engine::resources::ResourcesController>()->model("tree_winter");
    MyModel mtree1 =MyModel(tree1,glm::vec3(0.0f, -0.051, 0.5f),1,0.0f,0.0f,0.0f);
    //models.push_back(mtree1);


    auto tree2 = engine::core::Controller::get<engine::resources::ResourcesController>()->model("tree2");
    MyModel mtree2 =MyModel(tree2,glm::vec3(-0.2, -0.05, 0.3f),0.8,0.0f,0.0f,0.0f);

    auto grass1 = engine::core::Controller::get<engine::resources::ResourcesController>()->model("grass");
    MyModel mgrass1 =MyModel(grass1,glm::vec3(0.0f, 0, 0),1,0.0f,0.0f,0.0f);
   // models.push_back(mgrass1);


     auto grass = engine::core::Controller::get<engine::resources::ResourcesController>()->model("grass");
     grass->setShininess(5);
     make_instances(grass,-4.5f,4.5f,-4.5f,4.5f,-0.04, -0.015, 10000);

    auto tree = engine::core::Controller::get<engine::resources::ResourcesController>()->model("tree2");
    tree->setShininess(8);
    make_instances(tree,-4.2f,4.2f,-4.2f,4.2f,-0.065, -0.045, 30);


}


void Scene::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox_bloom_shader");
    shader->use();
    shader->set_float("treshold",m_bloom_filter->getTreshold());
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}
void Scene::draw_lights() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("light_shader");
    shader->use();
    shader->set_float("treshold",m_bloom_filter->getTreshold());
    light_swarm->draw(shader);
    light_swarm2->draw(shader);
}
void Scene::set_width_height(int width, int height) {
    m_deferred_filter->initilizeBuffers((width), (height));
    m_bloom_filter->initilizeBuffers((width), (height));
}

std::vector<engine::graphics::Light> Scene::get_lights() {

    if (m_swarm_enabled) {
        std::vector<engine::graphics::Light> res(lights);
        res.insert( res.begin(),light_swarm->get_lights().begin(),light_swarm->get_lights().end());
        res.insert( res.begin(),light_swarm2->get_lights().begin(),light_swarm2->get_lights().end());
        return res;
    }
    return lights;
}

void Scene::draw_instanced() {


    auto grass = engine::core::Controller::get<engine::resources::ResourcesController>()->model("grass");
    auto tree = engine::core::Controller::get<engine::resources::ResourcesController>()->model("tree2");

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader= engine::core::Controller::get<engine::resources::ResourcesController>()->shader("geometry_instanced_shader");

    shader->use();
    engine::core::Controller::get<engine::graphics::GraphicsController>()->prepare_for_draw(shader);

    shader->set_mat4("umodel", scale(glm::mat4(1.0f), glm::vec3(1.f)));
    tree->draw_instanced(shader);

    shader->set_mat4("umodel", scale(glm::mat4(1.0f), glm::vec3(0.35f)));
    grass->draw_instanced(shader);

}

void Scene::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    std::vector<engine::graphics::Light> all_lights = get_lights();

    m_deferred_filter->setUpCanvas();
    draw_static_scene(m_deferred_filter->geometry_shader());
    draw_instanced();

    engine::resources::Shader * drawing_shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("deferred_bloom_aware_render");
    drawing_shader->use();
    drawing_shader->set_float("treshold",m_bloom_filter->getTreshold());
    drawing_shader->set_lights(all_lights);
    drawing_shader->set_vec3("viewPos",graphics->camera()->Position);

    if (m_bloom_enabled) {
        m_bloom_filter->setUpCanvas();
        m_bloom_filter->clearBuffers();
    }
    else {
        engine::graphics::OpenGL::bindFrameBuffer(0);
    }
    m_deferred_filter->render(drawing_shader);
    m_deferred_filter->blitDepth(graphics->perspective_params().Width, graphics->perspective_params().Height, m_bloom_filter->get_framebuffer_id());
    if (m_bloom_enabled) {
        m_bloom_filter->setUpCanvas();
    }
    if (m_swarm_enabled) {
        draw_lights();
    }
    draw_skybox();
    if (m_bloom_enabled) {
        m_bloom_filter->applyBlur();
        engine::graphics::OpenGL::bindFrameBuffer(0);
        m_bloom_filter->applyBloom();
    }


}


void Scene::draw_bloom() {
    auto grass = engine::core::Controller::get<engine::resources::ResourcesController>()->model("backpack");

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    std::vector<engine::graphics::Light> all_lights = get_lights();


    engine::resources::Shader * drawing_shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("bloom_aware_shader");
    drawing_shader->use();
    drawing_shader->set_float("treshold",m_bloom_filter->getTreshold());
    graphics->prepare_for_draw(drawing_shader);
    drawing_shader->set_lights(all_lights);
    drawing_shader->set_vec3("viewPos",graphics->camera()->Position);

    m_bloom_filter->setUpCanvas();

    draw_static_scene(drawing_shader);
    m_bloom_filter->applyBlur();

    engine::graphics::OpenGL::bindFrameBuffer(0);
    m_bloom_filter->applyBloom();

}

bool Scene::swarm_enabled() {
    return m_swarm_enabled;
}

void Scene::set_swarm_enabled(bool enabled) {
    m_swarm_enabled=enabled;
}

void Scene::set_threshold(float threshold) {
    m_bloom_filter->set_threshold(threshold);
}

void Scene::set_bloom_intensity(float brightness) {
    m_bloom_filter->setIntensity(brightness);
}


void Scene::make_instances(engine::resources::Model* model,float fromx, float tox, float fromz, float toz, float fromy,float toy, uint32_t count=1000) {
    glm::mat4* trans=new glm::mat4[count];
    float inner_radius=(tox-fromx)/5.0f;
    float mitter_radius=(tox-fromx)/3.0f;
    float inner_prob=0.2f;
    float mitter_prob=0.7f;

    float max_deg_pitch=5.0f;
    float max_deg_yaw=180.0f;

    srand(engine::core::Controller::get<engine::platform::PlatformController>()->frame_time().current);


    int curr_count=0;
    while (curr_count<count) {
        const float x = fromx + (std::rand()*1.0f) / (RAND_MAX*1.0f) * (tox - fromx);
        const float z = fromz + (std::rand()*1.0f) / (RAND_MAX*1.0f) * (toz - fromz);
        const float y = fromy + (std::rand()*1.0f) / (RAND_MAX*1.0f) * (toy - fromy);

        float dist=std::sqrt(x*x+z*z);
        float prob=0.0f;
        if (dist<inner_radius) {
            prob=inner_prob;
        }
        else if (dist<mitter_radius) {
            prob=mitter_prob;
        }
        else {
            prob=1.0f;
        }
        float roll=(std::rand()*1.0f) / (RAND_MAX*1.0f);
        if (roll<prob) {
            glm::mat4 model_mat = glm::mat4(1.0f);
            model_mat = glm::translate(model_mat, glm::vec3(x, y, z));
            float scale = 0.8f  + (std::rand()*1.0f) / (RAND_MAX*1.0f) * 0.2f;
            model_mat = glm::scale(model_mat, glm::vec3(scale));

            float yaw= glm::radians(-max_deg_yaw +  (std::rand()*1.0f) / (RAND_MAX*1.0f) *  (max_deg_yaw*2));
            float pitch= glm::radians(-max_deg_pitch +  (std::rand()*1.0f) / (RAND_MAX*1.0f) *  (2*max_deg_pitch));

            model_mat = glm::rotate(model_mat, (yaw),   glm::vec3(0.0f, 1.0f, 0.0f));
            model_mat = glm::rotate(model_mat, (pitch), glm::vec3(1.0f, 0.0f, 0.0f));

            trans[curr_count++]=model_mat;

        }
    }

    model->instantiate(trans,count);
    delete[] trans;
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

std::vector<engine::graphics::Light>  Scene::scatter_lights(float fromx, float tox, float fromy,  float toy, float fromz, float toz, uint32_t count) {

    std::srand(static_cast<unsigned int>(engine::core::Controller::get<engine::platform::PlatformController>()->frame_time().current));

    std::vector<engine::graphics::Light> generated;
    generated.reserve(count);

    for (int i = 0; i < count; ++i) {
        const float x = fromx + (std::rand()*1.0f) / (RAND_MAX*1.0f) * (tox - fromx);
        const float y = fromy + (std::rand()*1.0f) / (RAND_MAX*1.0f) * (toy - fromy);
        const float z = fromz + (std::rand()*1.0f) / (RAND_MAX*1.0f) * (toz - fromz);


        const float rx = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
        const float ry = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;
        const float rz = 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 1.0f;

        glm::vec3 a= glm::normalize(glm::vec3(x, y, z));
        glm::vec3 b(rx,ry *0.3 ,rz);
        b=normalize(b);

        glm::vec3 proj = (glm::dot(b, a) / glm::dot(a, a)) * a;
        glm::vec3 b_ortho = b - proj;

        const float cr = -0.2 + (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX))*0.4f;
        const float cg = -0.4 + (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX))*0.4f;
        const float cb = -0.2 + (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX))*0.4f;

        glm::vec3 light_col=glm::vec3(0.7f+cr, 1.0f+cg, 0.3f+cb);

        engine::graphics::Light light(
            engine::graphics::LightType::Point,
            glm::vec3(x, y, z),
            light_col*0.5f
        );
        light.set_brightness(0.5);
        light.set_direction(b_ortho);
        light.set_attenuation(1, 0.4f, 0.5f);
        generated.push_back(light);
    }
    return generated;
}

}// namespace app