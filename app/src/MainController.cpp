//
// Created by miona on 2/18/25.
//

#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include "../include/MainController.hpp"
#include <spdlog/spdlog.h>
#include "GUIController.hpp"

namespace app {

    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto guiController = engine::core::Controller::get<GUIController>();
        if (!guiController->is_enabled()) {
            auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
            camera->rotate_camera(position.dx, position.dy);
        }
    }

    void MainController::initialize() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
        engine::graphics::OpenGL::enable_depth_testing();
        spdlog::info("MainController initialized");
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::updateCamera() {
        auto guiController = engine::core::Controller::get<GUIController>();
        if (guiController->is_enabled()) {
            return;
        }
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();

        float dt = platform->dt();
        if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
        }

    }

    void MainController::update() {
        updateCamera();
    }

    void MainController::drawBackpack() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *backpack = resources->model("backpack");
        engine::resources::Shader *shader  = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        model           = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);

        backpack->draw(shader);
    }

    void MainController::drawCar() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *car = resources->model("car");
        engine::resources::Shader *shader  = resources->shader("car");
        shader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        model           = glm::scale(model, glm::vec3(0.003f));
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->set_mat4("model", model);
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat3("normalMatrix", normalMatrix);


        glm::vec3 dirLightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        glm::vec3 dirLightCol = glm::vec3(1.0f, 1.0f, 0.95f);
        glm::vec3 dirLightAmb = glm::vec3(0.3f, 0.3f, 0.35f);
        shader->set_vec3("dirLightDir", dirLightDir);
        shader->set_vec3("dirLightCol", dirLightCol);
        shader->set_vec3("dirLightAmb", dirLightAmb);

        glm::vec3 pointLightPos = glm::vec3(0.0f, 1.0f, -5.0f);
        glm::vec3 pointLightCol = glm::vec3(2.0f, 1.8f, 1.6f);
        glm::vec3 pointLightAmb = glm::vec3(0.1f, 0.1f, 0.1f);
        shader->set_vec3("pointLightPos", pointLightPos);
        shader->set_vec3("pointLightCol", pointLightCol);
        shader->set_vec3("pointLightAmb", pointLightAmb);
        shader->set_float("c", 1.0f);
        shader->set_float("l", 0.22f);
        shader->set_float("q", 0.2f);

        shader->set_vec3("carMin", glm::vec3(-0.5f, 0.0f, -3.5f));
        shader->set_vec3("carMax", glm::vec3(0.5f, 0.5f, -2.5f));

        car->draw(shader);
    }

    void MainController::drawHouse() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *house = resources->model("farmHouse");
        engine::resources::Shader *shader  = resources->shader("basic");
        shader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, 0.0f, -6.0f));
        model           = glm::scale(model, glm::vec3(0.1f));
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->set_mat4("model", model);
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat3("normalMatrix", normalMatrix);

        glm::vec3 dirLightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        glm::vec3 dirLightCol = glm::vec3(1.0f, 1.0f, 0.95f);
        glm::vec3 dirLightAmb = glm::vec3(0.3f, 0.3f, 0.35f);
        shader->set_vec3("dirLightDir", dirLightDir);
        shader->set_vec3("dirLightCol", dirLightCol);
        shader->set_vec3("dirLightAmb", dirLightAmb);

        house->draw(shader);
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::drawSkybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox = resources->skybox("l_skybox");
        auto shader = resources->shader("skybox");
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->draw_skybox(shader, skybox);
    }

    void MainController::draw() {
        // drawBackpack();
        drawCar();
        drawHouse();
        drawSkybox();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

} // namespace app
