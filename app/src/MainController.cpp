//
// Created by teodora on 4.7.25..
//

#include "GuiController.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"
#include "spdlog/spdlog.h"
#include <MainController.hpp>


namespace app {

    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto gui_controller = engine::core::Controller::get<GUIController>();
        if (!gui_controller->is_enabled()) {
            auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
            camera->rotate_camera(position.dx, position.dy);
        }
    }

    void MainController::initialize() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
        engine::graphics::OpenGL::enable_depth_testing();

        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        camera->Position = glm::vec3(0.0f, 0.0f, 0.3f);
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::draw_floor() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *floorModel = resources->model("floor");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();

        setup_lighting();

        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 modelPod = glm::mat4(1.0f);
        modelPod = glm::translate(modelPod, glm::vec3(0.0f, -2.0f, -3.0f));
        modelPod = glm::rotate(modelPod, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader->set_mat4("model", modelPod);
        floorModel->draw(shader);
    }

    void MainController::draw_bench() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *benchModel = resources->model("bench");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();

        setup_lighting();

        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 modelKlupa = glm::mat4(1.0f);
        modelKlupa = glm::translate(modelKlupa, glm::vec3(0.3f, -2.0f, -3.0f));
        modelKlupa = glm::scale(modelKlupa, glm::vec3(0.012f));
        shader->set_mat4("model", modelKlupa);
        benchModel->draw(shader);
    }

    void MainController::update_camera() {
        auto gui_controller = engine::core::Controller::get<GUIController>();
        if (gui_controller->is_enabled()) {
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

    void MainController::update_spotlight() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_L).state() == engine::platform::Key::State::JustPressed) {
            spotlightEnabled = !spotlightEnabled;
        }
    }

    void MainController::update() {
        update_camera();
        update_spotlight();
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw_toy() {
        if (!spotlightEnabled)
            return;
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *toyModel = resources->model("toy");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();

        setup_lighting();

        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 modelIgracka = glm::mat4(1.0f);
        modelIgracka = glm::translate(modelIgracka, glm::vec3(-0.45f, -1.045, -2.8f));
        modelIgracka = glm::scale(modelIgracka, glm::vec3(0.4f));
        shader->set_mat4("model", modelIgracka);
        toyModel->draw(shader);
    }

    void MainController::draw_streetlamp() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *lampModel = resources->model("streetlamp");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();

        setup_lighting();

        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 modelLampa = glm::mat4(1.0f);
        modelLampa = glm::translate(modelLampa, glm::vec3(-1.6f, 0.5f, -3.0f));
        modelLampa = glm::rotate(modelLampa, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelLampa = glm::scale(modelLampa, glm::vec3(2.5f));
        shader->set_mat4("model", modelLampa);
        lampModel->draw(shader);
    }

    void MainController::setup_lighting() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Shader *modelShader = resources->shader("basic");

        modelShader->use();

        modelShader->set_vec3("dirLight.direction", glm::vec3(0.8f, -1.0f, 1.0f));
        modelShader->set_vec3("dirLight.ambient", glm::vec3(0.4f, 0.4f, 0.35f)*2.5f);
        modelShader->set_vec3("dirLight.diffuse", glm::vec3(0.6f, 0.5f, 0.5f)*0.5f);
        modelShader->set_vec3("dirLight.specular",  glm::vec3(0.4f, 0.4f, 0.3f));

        modelShader->set_vec3("spotLight.position", glm::vec3(-1.4f, 5.5f, -3.0f));
        modelShader->set_vec3("spotLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
        modelShader->set_float("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        modelShader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

        modelShader->set_float("spotLight.constant", 1.0f);
        modelShader->set_float("spotLight.linear", 0.045f);
        modelShader->set_float("spotLight.quadratic", 0.0075f);

        if (spotlightEnabled) {
            modelShader->set_vec3("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.1f));
            modelShader->set_vec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 0.8f));
            modelShader->set_vec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        } else {
            modelShader->set_vec3("spotLight.ambient", glm::vec3(0.0f));
            modelShader->set_vec3("spotLight.diffuse", glm::vec3(0.0f));
            modelShader->set_vec3("spotLight.specular", glm::vec3(0.0f));
        }

        modelShader->set_float("material_shininess", 32.0f);
        modelShader->set_vec3("viewPos", graphics->camera()->Position);
    }

    void MainController::draw() {
        draw_floor();
        draw_bench();
        draw_streetlamp();
        draw_toy();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

    }// namespace app