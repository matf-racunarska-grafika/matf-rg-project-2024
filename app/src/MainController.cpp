//
// Created by masa on 29/01/25.
//

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include "../include/MainController.hpp"

#include <spdlog/spdlog.h>

#include "../../engine/libs/assimp/code/AssetLib/3MF/3MFXmlTags.h"

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

    void MainController::draw_temple() {
        //Model
        auto resources                  = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* temple = resources->model("temple");
        // Shader
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();

        shader->set_mat4("projection",graphics->projection_matrix());
        shader->set_mat4("view",graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.9f));

        shader->set_mat4("model",model);


        temple->draw(shader);
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();

    }

    void MainController::draw() {
        //clear buffers
        draw_temple();
        //swap buffers
    }
} // app/