#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <memory>
#include <spdlog/spdlog.h>

#include <MainController.hpp>

#include "../../engine/libs/stb/include/stb_image.h"

namespace app {
    void MainController::initialize() {
        spdlog::info("MainController initialized");
        engine::graphics::OpenGL::enable_depth_testing();
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::draw_skull() {
        // Model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* skullModel = resources->model("skull");

        // Shader
        engine::resources::Shader* shader = resources->shader("BasicShader");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -34.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        shader->set_mat4("model", model);

        shader->set_vec3("pointLights[0].position", glm::vec3(0.0f, 3.0f, 3.0f));
        shader->set_vec3("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        shader->set_vec3("pointLights[0].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_float("pointLights[0].constant", 1.0f);
        shader->set_float("pointLights[0].linear", 0.09f);
        shader->set_float("pointLights[0].quadratic", 0.032f);
        shader->set_vec3("viewPosition", graphics->camera()->Position);
        shader->set_float("material.shininess", 32.0f);

        skullModel->draw(shader);
    }


    void MainController::draw() {

        draw_skull();
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }
} // app