//
// Created by miona on 2/18/25.
//

#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include "../include/MainController.hpp"
#include <spdlog/spdlog.h>
#include "GUIController.hpp"

namespace app {

    Light pointLight;
    Light dirLight;
    std::vector<Event> events;

    glm::vec3 birdPosition = glm::vec3(0.0f, 2.0f, -4.0f);
    float birdRadius = 2.0f;
    float birdSpeed = 1.0f;
    float birdAngle = 0.0f;
    float birdHeight = 2.0f;
    bool birdFlying = false;

    bool catVisible = false;
    bool dogVisible = true;

    bool cPressedLastFrame = false;

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

        pointLight = {
            glm::vec3(0.0f, 1.0f, -5.0f),
            glm::vec3(2.0f, 1.8f, 1.6f),
            glm::vec3(0.1f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            1.0f, 0.22f, 0.2f,
            true
        };

        dirLight = {
            glm::vec3(0.0f),
            glm::vec3(1.0f, 1.0f, 0.95f),
            glm::vec3(0.3f, 0.3f, 0.35f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            1.0f, 0.0f, 0.0f,
            true
        };

        events.push_back({
            []() {
                spdlog::info("Bird started flying!");
                birdFlying = true;
            },0.0f, false});

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

        if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) {
            pointLight.color *= 1.05f;
        }
        if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) {
            pointLight.color *= 0.95f;
        }
        if (platform->key(engine::platform::KeyId::KEY_RIGHT).is_down()) {
            pointLight.position.x += 0.1f;
        }
        if (platform->key(engine::platform::KeyId::KEY_LEFT).is_down()) {
            pointLight.position.x -= 0.15f;
        }

        bool cPressed = platform->key(engine::platform::KeyId::KEY_C).is_down();
        if (cPressed && !cPressedLastFrame) {
            spdlog::info("C pressed: Dog disappears, cat appears...");
            catVisible = true;
            dogVisible = false;
            events.push_back({
                []() {
                    spdlog::info("Dog appears again, cat disappears...");
                    dogVisible = true;
                    catVisible = false;
                }, 3.0f, false});
        }
        cPressedLastFrame = cPressed;
    }

    void MainController::updateEvents(float dt) {
        for (auto& event : events) {
            event.triggerTime -= dt;
            if (event.triggerTime <= 0.0f && !event.triggered) {
                event.action();
                event.triggered = true;
            }
        }
    }

    void MainController::updateBird(float dt) {
        if (!birdFlying) {
            return;
        }

        birdAngle += birdSpeed * dt;
        if(birdAngle > 2 * glm::pi<float>()) {
            birdAngle -= 2 * glm::pi<float>();
        }

        birdPosition.x = birdRadius * cos(birdAngle);
        birdPosition.z = birdRadius * sin(birdAngle) - 4.0f;
        birdPosition.y = birdHeight;
    }

    void MainController::update() {
        updateCamera();
        auto dt = engine::core::Controller::get<engine::platform::PlatformController>()->dt();
        updateEvents(dt);
        updateBird(dt);
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

        shader->set_vec3("dirLightDir", dirLight.direction);
        shader->set_vec3("dirLightCol", dirLight.color);
        shader->set_vec3("dirLightAmb", dirLight.ambient);

        shader->set_vec3("pointLightPos", pointLight.position);
        shader->set_vec3("pointLightCol", pointLight.color);
        shader->set_vec3("pointLightAmb", pointLight.ambient);
        shader->set_float("c", pointLight.c);
        shader->set_float("l", pointLight.l);
        shader->set_float("q", pointLight.q);

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

        shader->set_vec3("dirLightDir", dirLight.direction);
        shader->set_vec3("dirLightCol", dirLight.color);
        shader->set_vec3("dirLightAmb", dirLight.ambient);

        house->draw(shader);
    }

    void MainController::drawBird() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *bird = resources->model("bird");
        engine::resources::Shader *shader  = resources->shader("basic");
        shader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, birdPosition);
        model           = glm::scale(model, glm::vec3(0.1f));
        float modelForwardOffset = glm::radians(90.0f);
        float yaw = -birdAngle + glm::half_pi<float>() + modelForwardOffset;
        float pitch = glm::radians(35.0f);
        model = glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->set_mat4("model", model);
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat3("normalMatrix", normalMatrix);

        bird->draw(shader);
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

    void MainController::drawCat() {
        if (!catVisible) {
            return;
        }

        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *cat = resources->model("cat");
        engine::resources::Shader *shader  = resources->shader("basic");
        shader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(1.0f, 0.0f, -4.0f));
        model           = glm::scale(model, glm::vec3(0.009f));
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->set_mat4("model", model);
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat3("normalMatrix", normalMatrix);

        cat->draw(shader);
    }

    void MainController::drawDog() {
        if (!dogVisible) {
            return;
        }

        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *dog = resources->model("dog");
        engine::resources::Shader *shader  = resources->shader("basic");
        shader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(-1.0f, 0.0f, -4.0f));
        model           = glm::scale(model, glm::vec3(0.005f));
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->set_mat4("model", model);
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat3("normalMatrix", normalMatrix);

        dog->draw(shader);
    }

    void MainController::draw() {
        drawCar();
        drawHouse();
        drawSkybox();
        drawBird();
        drawCat();
        drawDog();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

} // namespace app
