//
// Created by aleksa on 1.6.25..
//

#include "MainController.h"
#include<memory>
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/resources/ResourcesController.hpp>
#include<GuiController.hpp>
#include "spdlog/spdlog.h"
#include <glm/gtc/random.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>

namespace app {

void MainPlatformEventObserver::on_key(engine::platform::Key key) { spdlog::info("Keyboard event: key={}, state={}", key.name(), key.state_str()); };

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    if (!gui_controller->is_enabled()) {
        auto camera = graphics->camera();
        camera->rotate_camera(position.dx, position.dy);
    }

};

void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();

    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(
            std::move(observer));

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    camera->Yaw = -90.0f;
    camera->Pitch = -5.79f;
    camera->rotate_camera(0, 0);
    camera->Position = glm::vec3(1.0f, 7.6f, 13.3f);


};

bool MainController::loop() {
    auto platform = get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).is_down()) { return false; }
    return true;
}

void MainController::update_camera() {
    auto gui = engine::core::Controller::get<GUIController>();
    if (gui->is_enabled()) return;

    auto platform = get<engine::platform::PlatformController>();
    auto graphics = get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();


    float dt = platform->dt();
    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt); }
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt); }
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt); }
    if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::UP, dt); }
    if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt); }


    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    auto skybox = resources->skybox("night_skybox");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::end_draw() {
    auto platform = get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MainController::set_up_shader_uniforms(engine::resources::Shader *shader, bool dirL, bool pointL) {
    auto graphics = get<engine::graphics::GraphicsController>();

    shader->set_float("emissiveStrength", emission_strength);
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("viewPos", graphics->camera()->Position);


    if (dirL) {
        shader->set_vec3("dirLight.direction", dirLightDirection);
        shader->set_vec3("dirLight.diffuse", dirLightColorDiffuse);
        shader->set_vec3("dirLight.ambient", dirLightColorAmbient);
        shader->set_vec3("dirLight.specular", dirLightColorSpecular);
    }

    if (pointL) {
        shader->set_vec3("pointLight.position", sun_pos);
        shader->set_vec3("pointLight.ambient", pointLightColorAmbient);
        shader->set_vec3("pointLight.specular", pointLightColorSpecular);
        shader->set_vec3("pointLight.diffuse", pointLightColorDiffuse);
        shader->set_float("pointLight.constant", pointLightConstant);
        shader->set_float("pointLight.linear", pointLightLinear);
        shader->set_float("pointLight.quadratic", pointLightQuadratic);
    }
}


void MainController::draw_sun() {
    auto resources = get<engine::resources::ResourcesController>();


    engine::resources::Model *sun = resources->model("sun");
    engine::resources::Shader *shader = resources->shader("default");
    shader->use();
    shader->set_vec3("emissive", emissive_color);
    set_up_shader_uniforms(shader, false, false);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, sun_pos);
    model = glm::rotate(model, glm::radians(sun_angle), sun_rot);

    model = glm::scale(model, glm::vec3(sun_scale));
    shader->set_mat4("model", model);

    sun->draw(shader);

}
void MainController::draw_ufo() {

    auto resources = get<engine::resources::ResourcesController>();


    engine::resources::Model *ufo = resources->model("ufo");
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    set_up_shader_uniforms(shader, true, true);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, ufo_pos);
    model = glm::rotate(model, glm::radians(ufo_angle), ufo_rot);

    model = glm::scale(model, glm::vec3(ufo_scale));
    shader->set_mat4("model", model);

    ufo->draw(shader);

}



void MainController::register_button() {
    auto platform = get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KEY_SPACE).state() == engine::platform::Key::State::JustPressed) { sequence_active = !sequence_active; }

};

void MainController::update_scene() {
    auto platform = get<engine::platform::PlatformController>();

    float dt = platform->dt();
    if (sequence_active) {
        if (sun_lights_up_timer < 3.0f && ufo_visible) {
            sun_lights_up_timer += dt;
            float t = glm::clamp(sun_lights_up_timer / 3.0f, 0.0f, 1.0f);


            emission_strength = glm::mix(emission_strength_start, 5.0f, t);
        } else {
            sun_starts_expanding_timer += dt;


            double pulse = (sin(timer_for_scaling * glm::pi<float>())) / 2.0f;
            timer_for_scaling += dt;
            if (sun_scale < 2.1f) { sun_scale += dt / 2.0f; }
            emission_strength = glm::mix(5.0f, 10.0f, pulse);


            if (sun_starts_expanding_timer <= 3.0f) { ufo_angle += 30.0f * dt; } else {
                glm::vec3 dir_from_sun = glm::normalize(ufo_pos - sun_pos);
                float speed = 7.0f;
                ufo_pos += dir_from_sun * speed * dt;

                float dist = glm::distance(ufo_pos, sun_pos);
                if (timer_for_scaling > 10.0f) ufo_visible = false;
            }


            if (timer_for_scaling >= 15.0f) {
                timer_for_scaling = 0.0f;
                sequence_active = false;
                not_blocked = true;
            }
        }
    } else {
        ufo_visible = true;
        sun_scale = sun_scale_start;
        ufo_angle = -41.0f;
        ufo_pos = ufo_pos_start;
        sun_lights_up_timer = 0.0f;
        sun_starts_expanding_timer = 0.0f;

        if (not_blocked) {
            emission_strength = emission_strength_start;
            not_blocked = false;
        }
    }
}

void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    camera->zoom(position.dy);
    graphics->perspective_params().FOV = glm::radians(camera->Zoom);
}

void MainController::draw() {
    update_camera();
    register_button();
    update_scene();
    draw_sun();
    if(ufo_visible)
        draw_ufo();
    draw_skybox();
}
}// app