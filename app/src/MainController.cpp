//
// Created by masa on 29/01/25.
//

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include "../include/MainController.hpp"

#include <GuiController.hpp>
#include <spdlog/spdlog.h>
#include <wayland-client-core.h>

#include "../../engine/libs/assimp/code/AssetLib/3MF/3MFXmlTags.h"

namespace engine::test::app {
    class GUIController;
}
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
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::update_camera() {
        auto gui_controller = engine::core::Controller::get<GUIController>();
        if (gui_controller->is_enabled())
            return;
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        platform->set_enable_cursor(false);
        auto camera = graphics->camera();
        float dt    = platform->dt();
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
        if (platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).is_down()) {
            camera->MovementSpeed = 5.0f;
        }
        else if (platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).is_up()) {
            camera->MovementSpeed = 2.5f;
        }

    }

    void MainController::update() {
        update_camera();
    }

    void MainController::draw_terrain() {
        auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *terrain = resources->model("terrain");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(7.0f, -11.0f, -1.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        shader->set_mat4("model", model);

        terrain->draw(shader);
    }
    void MainController::draw_monument() {
        auto resources                     = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                      = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *monument = resources->model("monument");
        engine::resources::Shader *shader  = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("model", glm::mat4(1.0f));
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(7.0f, -10.6f, -1.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        shader->set_mat4("model", model);

        monument->draw(shader);
    }

    void MainController::draw_columns() {
        auto resources                     = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                      = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *column = resources->model("column");
        engine::resources::Shader *shader  = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model1 = glm::mat4(1.0f);
        glm::mat4 model2 = glm::mat4(1.0f);
        glm::mat4 model3 = glm::mat4(1.0f);
        glm::mat4 model4 = glm::mat4(1.0f);

        model1 = glm::translate(model1, glm::vec3(3.5f, -10.5f, 3.0f)); //front left
        model1 = glm::scale(model1, glm::vec3(0.4f));

        model2 = glm::translate(model2, glm::vec3(10.0f, -10.9f, 3.0f)); //back left
        model2 = glm::scale(model2, glm::vec3(0.4f));

        model3 = glm::translate(model3, glm::vec3(3.5f, -10.4f, -4.8f)); //front right
        model3 = glm::scale(model3, glm::vec3(0.4f));

        model4 = glm::translate(model4, glm::vec3(10.0f, -10.8f, -4.8f)); //back right
        model4 = glm::scale(model4, glm::vec3(0.4f));

        shader->set_mat4("model", model1);
        column->draw(shader);

        shader->set_mat4("model", model2);
        column->draw(shader);

        shader->set_mat4("model", model3);
        column->draw(shader);

        shader->set_mat4("model", model4);
        column->draw(shader);

    }


    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox    = resources->skybox("sky");
        auto shader    = resources->shader("skybox");
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->draw_skybox(shader, skybox);
    }

    void MainController::draw() {
        draw_terrain();
        draw_monument();
        draw_columns();
        draw_skybox();
    }
} // namespace app
