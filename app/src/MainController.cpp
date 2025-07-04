//
// Created by teodora on 4.7.25..
//

#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"
#include "spdlog/spdlog.h"
#include <MainController.hpp>


namespace app {
    void MainController::initialize() {
        engine::graphics::OpenGL::enable_depth_testing();
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
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 modelPod = glm::mat4(1.0f);
        modelPod = glm::translate(modelPod, glm::vec3(0.0f, -2.0f, -3.0f));
        modelPod = glm::rotate(modelPod, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader->set_mat4("model", modelPod);
        floorModel->draw(shader);
    }
    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }
    void MainController::draw() {
        draw_floor();
    }
    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

    }// namespace app