//
// Created by matija on 10/12/25.
//

#include "../include/MySceneController.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <random>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <glm/gtx/quaternion.hpp>

namespace app {

void MySceneController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    //auto observer = std::make_unique<MainPlatformEventObserver>();
    //engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
}

bool MySceneController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) {
        return false;
    }
    return true;
}

void MySceneController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1)
                .state() == engine::platform::Key::State::JustPressed) {
        //platform->set_enable_cursor(false);
    }
}

void MySceneController::update() {
}

void MySceneController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MySceneController::draw() {
    draw_skybox();
}

void MySceneController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

void MySceneController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

void MySceneController::prepare_grass(float fromx, float tox, float fromy, float toy, uint32_t count=10000) {
    uint32_t num_of_inst=count;
    float radius = 5.0;
    float offset = 2.0f;
    float tilt_angle_bias=15.0f;

    auto grass_model=engine::core::Controller::get<engine::resources::ResourcesController>()->model("grass");
    glm::mat4* trans=new glm::mat4[num_of_inst];
    srand(engine::core::Controller::get<engine::platform::PlatformController>()->frame_time().current);


    for (uint32_t i=0; i<num_of_inst/5; i++) {

        for (uint32_t j=0; j<5; j++) {
            glm::mat4 model = glm::mat4(1.0f);
            float angle = (float)i / (float)num_of_inst * 360.0f;
            float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float x = sin(angle) * radius + displacement;
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float y = -abs(displacement) * 0.4f;
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float z = cos(angle) * radius + displacement;

            float scale =1+ (rand() % 20) / 100.0f;
            glm::vec3 dir=glm::vec3(0,1,0)+ glm::vec3(tan(tilt_angle_bias)) * glm::normalize(glm::vec3(x, y, z));
            glm::mat4 rot = glm::toMat4(glm::rotation(glm::vec3(0.0f,1.0f,0.0f), dir));

            model = glm::translate(model, glm::vec3(x, y, z));
            model = glm::scale(model, glm::vec3(scale));
            model=model*rot;


            model=glm::translate(model, glm::vec3(
                fromx + static_cast<float>(rand()) / RAND_MAX * (tox - fromx),
                fromy + static_cast<float>(rand()) / RAND_MAX * (toy - fromy),
                0.0f
                ));

            trans[i*5+j]=model;
        }

    }
    grass_model->instancite(trans,num_of_inst);
    delete[] trans;
}

void MySceneController::draw_grass() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("scene_shader");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto grass = engine::core::Controller::get<engine::resources::ResourcesController>()->model("grass");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    grass->draw(shader);
}


}



