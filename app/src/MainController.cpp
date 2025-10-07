//
// Created by filipn on 9/1/25.
//

#include <GuiController.hpp>
#include <MainController.hpp>

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<GuiController>();
    if (!gui_controller->is_enabled()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }
}

void MainController::initialize() {
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).is_down()) { return false; }
    return true;
}

void MainController::draw_moon() {
    //Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *moon = resources->model("moon");


    //Shader
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
    model = glm::scale(model, glm::vec3(0.7f));

    shader->set_mat4("model", model);
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    moon->draw(shader);
}

void MainController::draw_sun() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *sun = resources->model("sun");


    //Shader
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 20.0f));
    model = glm::scale(model, glm::vec3(0.7f));

    shader->set_mat4("model", model);
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    sun->draw(shader);
}

void MainController::draw_island() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *island = resources->model("island");


    //Shader
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-4.0f, 14.37f, -3.5f));
    model = glm::scale(model, glm::vec3(0.1f));

    shader->set_mat4("model", model);
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    island->draw(shader);
}

void MainController::draw_tree() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *tree = resources->model("tree");


    //Shader
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    glm::vec3 positions[] = {
            glm::vec3(0.0f, 0.0f, -0.5f),
            glm::vec3(-1.5f, 0.0f, -1.0f),
            glm::vec3(-1.0f, 0.0f, 1.0f),
            glm::vec3(0.5f, 0.0f, 0.8f),
            glm::vec3(1.5f, 0.0f, 0.2f),
            glm::vec3(1.0f, 0.0f, -1.5f),
            glm::vec3(-0.6f, 0.0f, -2.1f),
            glm::vec3(-2.5f, -1.515f, -0.6f),
            glm::vec3(-2.4f, -1.515f, -1.8f),
            glm::vec3(0.0f, -3.25f, -2.5f)
    };

    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    for (int i = 0; i < 10; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[i]);
        model = glm::rotate(model, glm::radians(i * 45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);

        tree->draw(shader);
    }
}

void MainController::draw_bench() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *bench = resources->model("bench");


    //Shader
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -0.28f));
    model = glm::scale(model, glm::vec3(0.05f));

    shader->set_mat4("model", model);
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    bench->draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.5f, 0.0f, -0.28f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.05f));
    shader->set_mat4("model", model);

    bench->draw(shader);
}

void MainController::draw_lamp() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *lamp = resources->model("lamp");


    //Shader
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    glm::vec3 positions[] = {
            glm::vec3(-1.0f, 0.0f, -0.28f),
            glm::vec3(0.0f, 0.0f, 0.67f),
            glm::vec3(1.0f, 0.0f, -0.28f),
            glm::vec3(0.0f, 0.0f, -1.23f)
    };

    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    for (int i = 0; i < 4; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[i]);
        model = glm::rotate(model, glm::radians(i * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        shader->set_mat4("model", model);

        lamp->draw(shader);
    }
}

void MainController::draw_bush() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *bush = resources->model("bush");


    //Shader
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    glm::vec3 positions[] = {
            glm::vec3(-0.2f, 0.13f, 0.1f),
            glm::vec3(0.2f, 0.13f, -2.0f),
            glm::vec3(-0.5f, 0.13f, 1.2f),
            glm::vec3(-0.7f, 0.13f, -1.5f),
            glm::vec3(0.9f, 0.13f, 1.0f),
            glm::vec3(-1.3f, 0.13f, 0.5f),
            glm::vec3(1.1f, 0.13f, -0.5f),
            glm::vec3(-1.6f, 0.13f, -0.3f),
            glm::vec3(1.7f, 0.13f, -1.1f),
            glm::vec3(-3.0f, -1.38f, -1.0f),
            glm::vec3(-2.3f, -1.38f, 0.1f),
            glm::vec3(-1.7f, -1.38f, -1.5f),
            glm::vec3(-0.7f, -3.11f, -1.9f),
            glm::vec3(0.5f, -3.11f, -2.1f),
            glm::vec3(-0.9f, -3.11f, -2.6f),
    };

    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    for (int i = 0; i < 15; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[i]);
        model = glm::rotate(model, glm::radians(i * 50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.0025f));
        shader->set_mat4("model", model);

        bush->draw(shader);
    }
}

void MainController::draw_path() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *path = resources->model("path");


    //Shader
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.8f, 0.0f, -0.5f));
    model = glm::scale(model, glm::vec3(0.3f));

    shader->set_mat4("model", model);
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    path->draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.8f, 0.0f, -0.5f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    shader->set_mat4("model", model);

    path->draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.8f, 0.0f, -0.075f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    shader->set_mat4("model", model);

    path->draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.8f, 0.0f, -0.075f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    shader->set_mat4("model", model);

    path->draw(shader);
}

void MainController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GuiController>();
    if (gui_controller->is_enabled()) { return; }

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KEY_W).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt); }
    if (platform->key(engine::platform::KEY_A).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }
    if (platform->key(engine::platform::KEY_S).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt); }
    if (platform->key(engine::platform::KEY_D).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt); }
    if (platform->key(engine::platform::KEY_SPACE).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::UP, dt); }
    if (platform->key(engine::platform::KEY_LEFT_SHIFT).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt); }


}

void MainController::update() { update_camera(); }

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("blue");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw() {
    draw_moon();
    draw_sun();
    draw_island();
    draw_tree();
    draw_bench();
    draw_lamp();
    draw_bush();
    draw_path();
    draw_skybox();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

}// app