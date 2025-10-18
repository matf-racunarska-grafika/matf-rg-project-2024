//
// Created by matfrg on 10/11/25.
//

#include "MainController.hpp"

#include "AppUtils.hpp"
#include "GuiController.hpp"
#include "spdlog/spdlog.h"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {
class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {

public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    if(is_gui_active()) {
        return;
    }
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx,position.dy);
}


void MainController::initialize() {
    spdlog::info("MainController initialized");
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    platform->set_enable_cursor(false);
    engine::graphics::OpenGL::enable_depth_testing();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}

void MainController::draw_island() {
    //Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *island = resources->model("ostrvo");
    //Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -2, 1));

    model = glm::scale(model, glm::vec3(0.7f));
    shader->set_mat4("model", model);

    island->draw(shader);
}

void MainController::draw_emissive_model(const std::string &model_name,
    const std::string &shader_name,
    const glm::vec3 &position,
    const glm::vec3 &scale,
    const glm::vec3 &rotation_axis,
    float rotation_angle) {

    //Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *model = resources->model(model_name);
    //Shader
    engine::resources::Shader *shader = resources->shader(shader_name);

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), position);

    if(rotation_angle!=0.0f && glm::length(rotation_axis)>0.0f) {
        model_matrix = glm::rotate(model_matrix,glm::radians(rotation_angle),rotation_axis);
    }

    model_matrix = glm::scale(model_matrix, scale);
    shader->set_mat4("model", model_matrix);

    if(shader_name == std::string("zvezda") && model_name == std::string("zvezda")) {
        shader->set_vec3("emissiveColor",m_star_color);
    }

    model->draw(shader);
}


void MainController::draw_lit_model(const std::string &model_name,
    const std::string &shader_name,
    const glm::vec3 &position,
    const glm::vec3 &scale,
    const glm::vec3 &rotation_axis,
    float rotation_angle) {

    //Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *model = resources->model(model_name);
    //Shader
    engine::resources::Shader *shader = resources->shader(shader_name);

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), position);
    //-0.95 1.27-1.2 2.02

    if(rotation_angle!=0.0f && glm::length(rotation_axis)>0.0f) {
        model_matrix = glm::rotate(model_matrix,glm::radians(rotation_angle),rotation_axis);
    }

    model_matrix = glm::scale(model_matrix, scale);
    shader->set_mat4("model", model_matrix);

    //direkciono svetlo
    shader->set_vec3("dirLight.direction",glm::vec3(0.03f,-0.76,-1.99));
    shader->set_vec3("dirLight.ambient",glm::vec3(0.05));
    shader->set_vec3("dirLight.diffuse",glm::vec3(0.45));
    shader->set_vec3("dirLight.specular",glm::vec3(0.5));

    //point zvezda
    glm::vec3 star_color = m_star_color;//glm::vec3(0.992f,0.875f,0.49f);
    shader->set_vec3("pointLights[0].position",glm::vec3(-0.93,1.48,2.02));
    shader->set_vec3("pointLights[0].ambient",glm::vec3(0.1f*star_color));
    shader->set_vec3("pointLights[0].diffuse",glm::vec3(0.6f*star_color));
    shader->set_vec3("pointLights[0].specular",glm::min(glm::vec3(1.0f),0.8f*star_color));
    shader->set_float("pointLights[0].constant",1.0f);
    shader->set_float("pointLights[0].linear",0.44f);
    shader->set_float("pointLights[0].quadratic",0.41f);

    //point logaorska vatra
    glm::vec3 fire_color = glm::vec3(0.8706f,0.5451f,0.0863f);
    float korektor_daljine = 1.3f; //>1 smanjuje daljinu svetla <1 povecava
    shader->set_vec3("pointLights[1].position",glm::vec3(-0.5,0.06,0.18));
    shader->set_vec3("pointLights[1].ambient",glm::vec3(0.1f*fire_color));
    shader->set_vec3("pointLights[1].diffuse",glm::vec3(0.6f*fire_color));
    shader->set_vec3("pointLights[1].specular",glm::min(glm::vec3(1.0f),0.8f*fire_color));
    shader->set_float("pointLights[1].constant",1.0f);
    shader->set_float("pointLights[1].linear",0.44f*korektor_daljine);
    shader->set_float("pointLights[1].quadratic",0.41f*korektor_daljine);

    shader->set_float("material.shininess",32);
    shader->set_vec3("viewPos",graphics->camera()->Position);

    model->draw(shader);

}

void MainController::draw_instanced_model(const std::string &model_name, const std::string &shader_name, int instance_count,
    float x_min, float x_max,
    float y_min, float y_max,
    float z_min, float z_max) {

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model* model = resources->model(model_name);
    engine::resources::Shader* shader = resources->shader(shader_name);

    //samo jednom se generise
    if(!m_seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        m_seeded = true;
    }

    InstanceData* instance = nullptr;
    for(auto& data : m_instances_data) {
        if(data.model_name == model_name) {
            instance = &data;
            break;
        }
    }
    //ako nije nadjen dodaj novi
    if(!instance) {
        InstanceData new_data;
        new_data.model_name = model_name;
        m_instances_data.push_back(std::move(new_data));
        instance = &m_instances_data.back();
    }

    if(!instance->initialized) {
        instance->matrices.clear();
        //pravljenje rand matrica u datim okvirima
        for(int i = 0; i<instance_count;++i) {
            float rand_x = x_min + (static_cast<float>(rand())/ RAND_MAX) * (x_max-x_min);
            float rand_y = y_min + (static_cast<float>(rand())/ RAND_MAX) * (y_max-y_min);
            float rand_z = z_min + (static_cast<float>(rand())/ RAND_MAX) * (z_max-z_min);

            glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f),
                glm::vec3(rand_x,rand_y,rand_z));
            model_matrix = glm::scale(model_matrix,glm::vec3(0.008f));
            instance->matrices.push_back(model_matrix);
        }



        model->set_instance_matrices(instance->matrices);
        instance->initialized=true;
    }
    //Shader
    shader->use();
    shader->set_mat4("projection",graphics->projection_matrix());
    shader->set_mat4("view",graphics->camera()->view_matrix());

    model->draw_instanced(shader);

}


void MainController::update_camera() {

    if(is_gui_active()) {
        return;
    }

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();
    if(platform->key(engine::platform::KeyId::KEY_W).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD,dt);
    }
    if(platform->key(engine::platform::KeyId::KEY_A).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT,dt);
    }
    if(platform->key(engine::platform::KeyId::KEY_S).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD,dt);
    }
    if(platform->key(engine::platform::KeyId::KEY_D).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT,dt);
    }
}


void MainController::update() {
    update_camera();
}


void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("space_skybox");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader,skybox);
}


void MainController::draw() {
    //draw_island();

    draw_lit_model("ostrvo","standard_lighting",glm::vec3(0,-2,1)
        ,glm::vec3(0.7f));
    draw_emissive_model("zvezda","zvezda",glm::vec3(-0.93,1.44,2.02)
        ,glm::vec3(0.1f));
    draw_lit_model("kuca","standard_lighting",glm::vec3(1.1,0,0.37)
        ,glm::vec3(0.1f),glm::vec3(0,1,0),-45.0);
    draw_lit_model("snesko","standard_lighting",glm::vec3(-0.27,0.04,2.87)
        ,glm::vec3(0.1f),glm::vec3(0,1,0),165.0);
    draw_lit_model("sanke","standard_lighting",glm::vec3(-1.33,-0.01,0.64)
        ,glm::vec3(0.1f));
    draw_lit_model("poklon","standard_lighting",glm::vec3(-0.88,-0.01,1.42)
        ,glm::vec3(0.008f),glm::vec3(1,0,0),-90);
    draw_lit_model("poklon","standard_lighting",glm::vec3(-1.2,-0.01,1.71)
        ,glm::vec3(0.008f),glm::vec3(1,0,0),-90);
    draw_lit_model("oblak","standard_lighting",glm::vec3(0.5f,10.0f,0.85f)
        ,glm::vec3(0.0041f),glm::vec3(1,0,0),90);
    draw_instanced_model("poklon","basic_instanced",5
        ,-3.0f,3.0f
        ,0.0f,9.5f
        ,-2.0f,5.0f);
    draw_emissive_model("bonfire","basic_rgba",glm::vec3(-0.5,-0.01,0.18)
        ,glm::vec3(0.05f));
    draw_skybox();

}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}


}// namespace app
