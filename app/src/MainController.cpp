//
// Created by masa on 29/01/25.
//

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include "../include/MainController.hpp"

#include <GuiController.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <wayland-client-core.h>

#include "../../engine/libs/assimp/code/AssetLib/3MF/3MFXmlTags.h"
#include "../../engine/libs/glad/include/glad/glad.h"
#include "../../engine/libs/glfw/include/GLFW/glfw3.h"

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
            camera->MovementSpeed = 12.0f;
        } else if (platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).is_up()) {
            camera->MovementSpeed = 8.0f;
        }
    }

    void MainController::update() {
        update_camera();
    }

    void MainController::draw_terrain() {
        auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera                       = graphics->camera();
        engine::resources::Model *terrain = resources->model("terrain");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();
        shader->set_vec3("LightPos", glm::vec3(21.0f, -5.0f, 2.0f));
        shader->set_vec3("LightColor", glm::vec3(10.0f,10.0f,10.0f));
        shader->set_vec3("moonLightDir", glm::vec3(0.0f, 30.0f, 25.0f));
        shader->set_vec3("moonLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        shader->set_vec3("viewPos", camera->Position);
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(20.0f, -15.0f, 0.0f));
        model           = glm::scale(model, glm::vec3(3.0f));
        shader->set_mat4("model", model);

        terrain->draw(shader);
    }

    void MainController::draw_gazebo() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera    = graphics->camera();

        engine::resources::Model *gazebo  = resources->model("gazebo");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();
        shader->set_vec3("LightPos", glm::vec3(21.0f, -5.0f, 2.0f));
        shader->set_vec3("LightColor", glm::vec3(10.0f,10.0f,10.0f));

        shader->set_vec3("moonLightDir", glm::vec3(0.0f, 30.0f, 25.0f));
        shader->set_vec3("moonLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        shader->set_vec3("viewPos", camera->Position);
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("model", glm::mat4(1.0f));
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(20.0f, -15.0f, 0.0f));
        float angle     = glm::radians(270.0);
        model           = glm::rotate(model, angle, glm::vec3(0, 1, 0));
        model           = glm::scale(model, glm::vec3(0.04f));
        shader->set_mat4("model", model);

        gazebo->draw(shader);
    }

    void MainController::draw_columns() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera    = graphics->camera();

        engine::resources::Model *column  = resources->model("column");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();
        shader->set_vec3("viewPos", camera->Position);
        shader->set_vec3("LightPos", glm::vec3(21.0f, -5.0f, 2.0f));
        shader->set_vec3("LightColor", glm::vec3(10.0f,10.0f,10.0f));

        shader->set_vec3("moonLightDir", glm::vec3(0.0f, 30.0f, 25.0f));
        shader->set_vec3("moonLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model1 = glm::mat4(1.0f);
        glm::mat4 model2 = glm::mat4(1.0f);
        glm::mat4 model3 = glm::mat4(1.0f);
        glm::mat4 model4 = glm::mat4(1.0f);

        model1 = glm::translate(model1, glm::vec3(6.5f, -15.0f, 13.0f)); // front left
        model1 = glm::scale(model1, glm::vec3(1.0f));

        model2 = glm::translate(model2, glm::vec3(34.5f, -15.0f, 13.0f)); // back left
        model2 = glm::scale(model2, glm::vec3(1.0f));

        model3 = glm::translate(model3, glm::vec3(6.5f, -15.0f, -12.5f)); // front right
        model3 = glm::scale(model3, glm::vec3(1.0f));

        model4 = glm::translate(model4, glm::vec3(34.5f, -15.0f, -12.5f)); // back right
        model4 = glm::scale(model4, glm::vec3(1.0f));

        for (auto &model: {model1, model2, model3, model4}) {
            shader->set_mat4("model", model);
            column->draw(shader);
        }
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

    void MainController::draw_butterfly_instanced() {
        auto resources                      = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                       = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *butterfly = resources->model("butterfly");
        engine::resources::Shader *shader   = resources->shader("instancing");
        auto camera                         = graphics->camera();

        unsigned int amount = 100;
        glm::mat4 *modelMatrices;
        modelMatrices = new glm::mat4[amount];
        srand(glfwGetTime());
        float radius        = 15.0;
        float offset        = 1.5f;
        glm::vec3 gazeboPos = glm::vec3(20.0f, -3.0f, 0.0f);

        for (unsigned int i = 0; i < amount; i++) {
            glm::mat4 model    = glm::mat4(1.0f);
            float angle        = (float) i / (float) amount * 360.0f;
            float displacement = (rand() % (int) (2 * offset * 100)) / 100.0f - offset;
            float x            = sin(angle) * radius + displacement;
            displacement       = (rand() % (int) (2 * offset * 100)) / 100.0f - offset;
            float y            = displacement * 0.4f;
            displacement       = (rand() % (int) (2 * offset * 100)) / 100.0f - offset;
            float z            = cos(angle) * radius + displacement;
            model              = glm::translate(model, gazeboPos + glm::vec3(x, y, z));

            float scale = 0.5f;
            model       = glm::scale(model, glm::vec3(scale));

            float rotAngle = static_cast<float>((rand() % 360));
            model          = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

            modelMatrices[i] = model;
        }

        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
        for (unsigned int i = 0; i < butterfly->meshes().size(); i++) {
            unsigned int VAO = butterfly->meshes()[i].m_vao;
            glBindVertexArray(VAO);
            std::size_t vec4Size = sizeof(glm::vec4);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) 0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (1 * vec4Size));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (2 * vec4Size));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (3 * vec4Size));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }

        shader->use();
        shader->set_vec3("LightPos", glm::vec3(7.0f, 30.0f, -1.0f));
        shader->set_vec3("viewPos", camera->Position);
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        for (unsigned int i = 0; i < butterfly->meshes().size(); i++) {
            glBindVertexArray(butterfly->meshes()[i].m_vao);

            for (unsigned int j = 0; j < butterfly->meshes()[i].m_textures.size(); j++) {
                glActiveTexture(GL_TEXTURE0 + j);
                glBindTexture(GL_TEXTURE_2D, butterfly->meshes()[i].m_textures[j]->id());
            }

            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(butterfly->meshes()[i].m_num_indices),
                                    GL_UNSIGNED_INT, 0, amount);
        }
    }
    void MainController::draw_garden() {
        auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera                       = graphics->camera();
        engine::resources::Model *garden  = resources->model("garden");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();
        shader->set_vec3("LightPos", glm::vec3(21.0f, -5.0f, 2.0f));
        shader->set_vec3("LightColor", glm::vec3(10.0f,10.0f,10.0f));

        shader->set_vec3("moonLightDir", glm::vec3(0.0f, 30.0f, 25.0f));
        shader->set_vec3("moonLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("viewPos", camera->Position);

        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        float angle     = glm::radians(270.0);
        float angle2    = glm::radians(180.0);
        model           = glm::translate(model, glm::vec3(30.0f, -24.0f, 0.0f));
        model           = glm::rotate(model, angle, glm::vec3(1, 0, 0));
        model           = glm::scale(model, glm::vec3(1.0f));
        shader->set_mat4("model", model);

        garden->draw(shader);
    }

    void MainController::draw_statue() {
        auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera                       = graphics->camera();
        engine::resources::Model *statue  = resources->model("statue");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();
        shader->set_vec3("LightPos", glm::vec3(21.0f, -5.0f, 2.0f));
        shader->set_vec3("LightColor", glm::vec3(10.0f,10.0f,10.0f));

        shader->set_vec3("moonLightDir", glm::vec3(0.0f, 30.0f, 25.0f));
        shader->set_vec3("moonLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        shader->set_vec3("viewPos", camera->Position);
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        float angle     = glm::radians(270.0);
        float angle2    = glm::radians(90.0);
        model           = glm::translate(model, glm::vec3(20.0f, -14.0f, 0.0f));
        model           = glm::rotate(model, angle, glm::vec3(0, 1, 0));
        model           = glm::rotate(model, angle, glm::vec3(1, 0, 0));
        model           = glm::scale(model, glm::vec3(0.5f));
        shader->set_mat4("model", model);

        statue->draw(shader);
    }

    void MainController::draw_temple() {
        auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera                       = graphics->camera();
        engine::resources::Model *temple  = resources->model("temple");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();
        shader->set_vec3("LightPos", glm::vec3(21.0f, -5.0f, 2.0f));
        shader->set_vec3("LightColor", glm::vec3(10.0f,10.0f,10.0f));

        shader->set_vec3("moonLightDir", glm::vec3(0.0f, 30.0f, 25.0f));
        shader->set_vec3("moonLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        shader->set_vec3("viewPos", camera->Position);
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(70.0f, -15.5f, -30.0f));
        model           = glm::scale(model, glm::vec3(2.0f));
        shader->set_mat4("model", model);

        temple->draw(shader);
    }

    void MainController::draw_tree() {
        auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera                       = graphics->camera();
        engine::resources::Model *tree    = resources->model("tree");
        engine::resources::Shader *shader = resources->shader("basic");

        shader->use();
        shader->set_vec3("LightPos", glm::vec3(21.0f, -5.0f, 2.0f));
        shader->set_vec3("LightColor", glm::vec3(10.0f,10.0f,10.0f));

        shader->set_vec3("moonLightDir", glm::vec3(-30.0f, 30.0f, 25.0f));
        shader->set_vec3("moonLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        shader->set_vec3("viewPos", camera->Position);
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(20.0f, -15.0f, -30.0f));
        model           = glm::scale(model, glm::vec3(1.0f));
        shader->set_mat4("model", model);

        tree->draw(shader);
    }

    void MainController::draw_light() {
        auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *light   = resources->model("light");
        engine::resources::Shader *shader = resources->shader("light");

        shader->use();
        shader->set_vec3("LightColor", glm::vec3(10.0f,10.0f,10.0f));
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(21.0f, -5.0f, 2.0f));
        model           = glm::scale(model, glm::vec3(7.0f));
        shader->set_mat4("model", model);

        light->draw(shader);
    }


    void MainController::draw() {
        draw_terrain();
        draw_gazebo();
        draw_columns();
        draw_butterfly_instanced();
        draw_tree();
        draw_statue();
        draw_temple();
        draw_garden();
        draw_light();
        draw_skybox();
    }
} // namespace app
