//
// Created by aleksa on 1.6.25..
//

#include "MainController.h"
#include "../../engine/libs/glad/include/glad/glad.h"
#include<memory>
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/resources/ResourcesController.hpp>
#include<GuiController.hpp>
#include "spdlog/spdlog.h"
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
    // camera->rotate_camera(glm::radians(-90.0f), glm::radians(-5.7f));


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

    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    auto skybox = resources->skybox("field_skybox");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw_goal() {
    //  Model
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();

    engine::resources::Model *goal = resources->model("goal");
    engine::resources::Shader *shader = resources->shader("goalShader");

    shader->use();
    shader->set_vec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader->set_vec3("lightPos", glm::vec3(0.0f, 10.0f, 0.0f));
    shader->set_vec3("viewPos", graphics->camera()->Position);
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(gol_x, gol_y, gol_z));
    model = glm::rotate(model, glm::radians(ugao_gol), glm::vec3(gosa_x, gosa_y, gosa_z));
    model = glm::scale(model, glm::vec3(gol_skalirano));
    shader->set_mat4("model", model);

    goal->draw(shader);

}

void MainController::draw_grass() {
    //  Model
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();

    engine::resources::Model *grass = resources->model("grass");
    //  Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(trava_x, trava_y, trava_z));
    model = glm::rotate(model, glm::radians(ugao_trava), glm::vec3(osa_x, osa_y, osa_z));
    model = glm::scale(model, glm::vec3(trava_skalirano));
    shader->set_mat4("model", model);
    grass->draw(shader);
}

void MainController::draw_ball() {
    //  Model
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();

    engine::resources::Model *ball = resources->model("ball");
    //  Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    // namesti y
    model = glm::translate(model, glm::vec3(lopta_x, lopta_y, lopta_z));

    model = glm::scale(model, glm::vec3(lopta_skalirano));
    shader->set_mat4("model", model);
    ball->draw(shader);

};

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::end_draw() {
    auto platform = get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MainController::draw() {
    draw_grass();
    draw_reflectors();
    draw_goalkeeper();
    draw_red_players();
    draw_blue_players();
    draw_goal();
    draw_ball();

    draw_skybox();

}

void MainController::draw_reflectors() {
    //  Model
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();

    engine::resources::Model *reflector = resources->model("reflector");
    //  Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    // Crtanje PRVOG reflektora
    glm::mat4 model1 = glm::mat4(1.0f);// Resetujemo matricu

    model1 = glm::translate(model1, glm::vec3(ref_x, ref_y, ref_z));
    model1 = glm::rotate(model1, glm::radians(ref1_ugao), glm::vec3(ref_xv, ref_yv, ref_zv));
    model1 = glm::scale(model1, glm::vec3(ref_skalirano));
    shader->set_mat4("model", model1);
    reflector->draw(shader);

    // Crtanje DRUGOG reflektora
    glm::mat4 model2 = glm::mat4(1.0f);// Resetujemo matricu
    model2 = glm::translate(model2, glm::vec3(ref2_x, ref2_y, ref2_z));
    model2 = glm::rotate(model2, glm::radians(ref2_ugao), glm::vec3(ref2_xv, ref2_yv, ref2_zv));
    model2 = glm::scale(model2, glm::vec3(ref_skalirano));
    shader->set_mat4("model", model2);
    reflector->draw(shader);
}

void MainController::draw_red_players() {
    //  Model
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();

    engine::resources::Model *red_player = resources->model("crveni");
    //  Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model[2];

    for (int i = 0; i < 2; i++) {
        model[i] = glm::mat4(1.0f);
        // namesti y
        model[i] = glm::translate(model[i], glm::vec3(crveni_x - i * 7.0f, crveni_y, crveni_z));
        model[i] = glm::rotate(model[i], glm::radians(ugao_crveni), glm::vec3(crveni_xv, crveni_yv, crveni_zv));
        model[i] = glm::scale(model[i], glm::vec3(crveni_skalirano));
        shader->set_mat4("model", model[i]);
        red_player->draw(shader);
    }

};

void MainController::draw_goalkeeper() {
    //  Model
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();

    engine::resources::Model *goalkeeper = resources->model("goalkeeper");
    //  Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    // namesti y
    model = glm::translate(model, glm::vec3(golman_x, golman_y, golman_z));
    model = glm::rotate(model, glm::radians(ugao_golman), glm::vec3(golman_xv, golman_yv, golman_zv));
    model = glm::scale(model, glm::vec3(golman_skalirano));
    shader->set_mat4("model", model);
    goalkeeper->draw(shader);

};

void MainController::draw_blue_players() {
    //  Model
    auto resources = get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();

    engine::resources::Model *blue_player = resources->model("plavi");
    //  Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model[4];

    for (int i = 0; i < 4; i++) {
        model[i] = glm::mat4(1.0f);
        // namesti y
        model[i] = glm::translate(model[i], glm::vec3(plavi_x + 0.65f * i, plavi_y, plavi_z));
        model[i] = glm::rotate(model[i], glm::radians(ugao_plavi), glm::vec3(plavi_xv, plavi_yv, plavi_zv));
        model[i] = glm::scale(model[i], glm::vec3(plavi_skalirano));
        shader->set_mat4("model", model[i]);
        blue_player->draw(shader);
    }


};

void MainController::update() { update_camera(); }

void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    camera->zoom(position.dy);
    graphics->perspective_params().FOV = glm::radians(camera->Zoom);
}

}// app