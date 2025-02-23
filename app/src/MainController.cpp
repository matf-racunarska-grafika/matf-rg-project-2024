#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <MainController.hpp>
#include <GUIController.hpp>

#include "../../engine/libs/glad/include/glad/glad.h"

namespace app {

    float test_scale      = 1.0f;
    float test_rotation_x = 0.0f;
    float test_rotation_y = 0.0f;
    float test_rotation_z = 0.0f;
    float test_x          = 0.0f;
    float test_y          = 0.0f;
    float test_z          = 0.0f;

    bool mouse_enabled;
    bool is_day;

    engine::resources::ResourcesController *resources;
    engine::graphics::GraphicsController *graphics;
    GUIController gui;
    engine::graphics::Camera *camera;

    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto gui = engine::core::Controller::get<GUIController>();
        if (gui->is_enabled())
            return;
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }

    void MainController::initialize() {
        engine::graphics::OpenGL::enable_depth_testing();
        graphics      = engine::core::Controller::get<engine::graphics::GraphicsController>();
        camera        = graphics->camera();
        resources     = engine::core::Controller::get<engine::resources::ResourcesController>();
        gui           = GUIController();
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
        mouse_enabled = false;
        platform->set_enable_cursor(mouse_enabled);
        is_day           = true;
        camera->Front    = glm::vec3(0.77, -0.08, -0.6);
        camera->Position = glm::vec3(5, 27, 17);
        camera->Yaw      = -38;
        camera->Pitch    = -5;

    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::end_draw() {
        engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
    }

    void MainController::draw() {
        draw_terrain();
        draw_campfire();
        draw_logs();
        draw_tents();
        draw_old_tree();
        draw_forest();
        draw_bushes();
        draw_flowers();
        draw_path();
        draw_mushrooms();
        test();
        draw_skybox();
    }

    void MainController::draw_forest() {
        engine::resources::Model *yellow_tree  = resources->model("yellow_tree");
        engine::resources::Model *green_tree   = resources->model("green_tree");
        engine::resources::Model *tall_tree    = resources->model("beech_tree");
        engine::resources::Model *oak_tree     = resources->model("oak_tree");
        engine::resources::Model *tree_gate    = resources->model("tree_gate");
        engine::resources::Model *pine_tree    = resources->model("pine_tree");
        engine::resources::Shader *tree_shader = resources->shader("tree_shader2");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        // Activate the shader.
        tree_shader->use();

        // Set the light properties.
        tree_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            tree_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            tree_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            tree_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            tree_shader->set_float("material.shininess", 32.0f);
        } else {
            tree_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            tree_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            tree_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            tree_shader->set_float("material.shininess", 128.0f);
        }
        tree_shader->set_vec3("viewPos", camera->Position);

        // Set the projection and view matrices.
        tree_shader->set_mat4("projection", graphics->projection_matrix());
        tree_shader->set_mat4("view", camera->view_matrix());

        glm::mat4 model = glm::mat4(1.0f);
        // model           = glm::rotate(model, glm::radians(test_rotation), glm::vec3(0, 1, 0));
        model = glm::translate(model, glm::vec3(49, 17, 9));
        model = glm::scale(model, glm::vec3(0.04));
        tree_shader->set_mat4("model", model);
        yellow_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        // model           = glm::rotate(model, glm::radians(test_rotation), glm::vec3(0, 1, 0));
        model = glm::translate(model, glm::vec3(0, 17, -22));
        model = glm::scale(model, glm::vec3(0.04));
        tree_shader->set_mat4("model", model);
        yellow_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        // model           = glm::rotate(model, glm::radians(test_rotation), glm::vec3(0, 1, 0));
        model = glm::translate(model, glm::vec3(-25, 15, 5));
        model = glm::scale(model, glm::vec3(0.04));
        tree_shader->set_mat4("model", model);
        yellow_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4, 17, 25));
        model = glm::scale(model, glm::vec3(0.04));
        tree_shader->set_mat4("model", model);
        yellow_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(39, 17, -5));
        model = glm::scale(model, glm::vec3(0.06));
        tree_shader->set_mat4("model", model);
        yellow_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(45, 17, 12));
        model = glm::scale(model, glm::vec3(0.06));
        tree_shader->set_mat4("model", model);
        yellow_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(37, 17, 32));
        model = glm::scale(model, glm::vec3(0.06));
        tree_shader->set_mat4("model", model);
        yellow_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(11, 17, 32));
        model = glm::scale(model, glm::vec3(0.06));
        tree_shader->set_mat4("model", model);
        yellow_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0, 0));
        model = glm::translate(model, glm::vec3(-10, 1, 17));
        model = glm::scale(model, glm::vec3(0.22f));
        tree_shader->set_mat4("model", model);
        green_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0, 0));
        model = glm::translate(model, glm::vec3(-2, 6, 16));
        model = glm::scale(model, glm::vec3(0.29f));
        tree_shader->set_mat4("model", model);
        green_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0, 0));
        model = glm::translate(model, glm::vec3(45, 1, 16));
        model = glm::scale(model, glm::vec3(0.29f));
        tree_shader->set_mat4("model", model);
        green_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0, 0));
        model = glm::translate(model, glm::vec3(-1, -39, 16));
        model = glm::scale(model, glm::vec3(0.29f));
        tree_shader->set_mat4("model", model);
        green_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0, 0));
        model = glm::translate(model, glm::vec3(8, 11, 17));
        model = glm::scale(model, glm::vec3(0.22f));
        tree_shader->set_mat4("model", model);
        green_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0, 0));
        model = glm::translate(model, glm::vec3(33, 10, 17));
        model = glm::scale(model, glm::vec3(0.22f));
        tree_shader->set_mat4("model", model);
        green_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0, 0));
        model = glm::translate(model, glm::vec3(17, -24, 17));
        model = glm::scale(model, glm::vec3(0.22f));
        tree_shader->set_mat4("model", model);
        green_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0, 0));
        model = glm::translate(model, glm::vec3(39, -24, 17));
        model = glm::scale(model, glm::vec3(0.22f));
        tree_shader->set_mat4("model", model);
        green_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6, 14, -15));
        model = glm::scale(model, glm::vec3(0.1f));
        tree_shader->set_mat4("model", model);
        tall_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-19, 14, -8));
        model = glm::scale(model, glm::vec3(0.1f));
        tree_shader->set_mat4("model", model);
        tall_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(21, 14, 38));
        model = glm::scale(model, glm::vec3(0.1f));
        tree_shader->set_mat4("model", model);
        tall_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(-17, 28, -17));
        model = glm::scale(model, glm::vec3(0.210f));
        tree_shader->set_mat4("model", model);
        oak_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(112.0f), glm::vec3(0, 1, 0));
        model = glm::translate(model, glm::vec3(8, 13, 59));
        model = glm::scale(model, glm::vec3(0.110f));
        tree_shader->set_mat4("model", model);
        tree_gate->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(32, 0, 17));
        model = glm::scale(model, glm::vec3(11.0f));
        tree_shader->set_mat4("model", model);
        pine_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(36, 22, 17));
        model = glm::scale(model, glm::vec3(11.0f));
        tree_shader->set_mat4("model", model);
        pine_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(12, 20, 17));
        model = glm::scale(model, glm::vec3(11.0f));
        tree_shader->set_mat4("model", model);
        pine_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(49, -9, 17));
        model = glm::scale(model, glm::vec3(11.0f));
        tree_shader->set_mat4("model", model);
        pine_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(23, -30, 17));
        model = glm::scale(model, glm::vec3(11.0f));
        tree_shader->set_mat4("model", model);
        pine_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(-5, -26, 17));
        model = glm::scale(model, glm::vec3(11.0f));
        tree_shader->set_mat4("model", model);
        pine_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(-11, 1, 17));
        model = glm::scale(model, glm::vec3(11.0f));
        tree_shader->set_mat4("model", model);
        pine_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(-26, 0, 13));
        model = glm::scale(model, glm::vec3(11.0f));
        tree_shader->set_mat4("model", model);
        pine_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(-12, 10, 17));
        model = glm::scale(model, glm::vec3(11.0f));
        tree_shader->set_mat4("model", model);
        pine_tree->draw(tree_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(26, 29, 17));
        model = glm::scale(model, glm::vec3(11.0f));
        tree_shader->set_mat4("model", model);
        pine_tree->draw(tree_shader);
    }

    void MainController::draw_campfire() {
        engine::resources::Model *campfire         = resources->model("campfire");
        engine::resources::Shader *campfire_shader = resources->shader("campfire_shader");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        if (is_day) {
            campfire_shader->use();

            campfire_shader->set_vec3("light.position", lightPos);
            campfire_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            campfire_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            campfire_shader->set_vec3("light.specular", glm::vec3(0.1f, 0.1f, 0.1f));
            campfire_shader->set_vec3("viewPos", camera->Position);

            campfire_shader->set_vec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f)); // specular highlights
            campfire_shader->set_float("material.shininess", 2048.0f);

            campfire_shader->set_mat4("projection", graphics->projection_matrix());
            campfire_shader->set_mat4("view", camera->view_matrix());

            glm::mat4 model = glm::mat4(1.0f);
            // model = glm::scale(model, glm::vec3(1));
            model = glm::translate(model, glm::vec3(12.0f, 17.3f, 6.0f));
            campfire_shader->set_mat4("model", model);

            campfire->draw(campfire_shader);
        } else {
            campfire_shader->use();

            campfire_shader->set_vec3("light.position", lightPos);
            campfire_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            campfire_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            campfire_shader->set_vec3("light.specular", glm::vec3(0.1f, 0.1f, 0.1f));
            campfire_shader->set_vec3("viewPos", camera->Position);

            campfire_shader->set_vec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f)); // specular highlights
            campfire_shader->set_float("material.shininess", 4096.0f);

            campfire_shader->set_mat4("projection", graphics->projection_matrix());
            campfire_shader->set_mat4("view", camera->view_matrix());

            glm::mat4 model = glm::mat4(1.0f);
            // model = glm::scale(model, glm::vec3(1));
            model = glm::translate(model, glm::vec3(12.0f, 17.3f, 6.0f));
            campfire_shader->set_mat4("model", model);

            campfire->draw(campfire_shader);
        }
    }

    void MainController::draw_logs() {
        engine::resources::Model *log_seat         = resources->model("log_seat");
        engine::resources::Shader *log_seat_shader = resources->shader("campfire_shader");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        // Activate the shader.
        log_seat_shader->use();

        // Set the light properties.
        log_seat_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            log_seat_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            log_seat_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            log_seat_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            log_seat_shader->set_float("material.shininess", 32.0f);
        } else {
            log_seat_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            log_seat_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            log_seat_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            log_seat_shader->set_float("material.shininess", 128.0f);
        }
        log_seat_shader->set_vec3("viewPos", camera->Position);

        // Set the projection and view matrices.
        log_seat_shader->set_mat4("projection", graphics->projection_matrix());
        log_seat_shader->set_mat4("view", camera->view_matrix());

        for (int i = 0; i < 3; i++) {
            if (i == 0) {
                glm::mat4 model = glm::mat4(1.0f);
                model           = glm::rotate(model, glm::radians(42.0f), glm::vec3(0, 1, 0));
                model           = glm::translate(model, glm::vec3(6, 17.5, 2));
                model           = glm::scale(model, glm::vec3(0.04));

                log_seat_shader->set_mat4("model", model);

                // Draw the campfire model.
                log_seat->draw(log_seat_shader);
            } else if (i == 1) {
                glm::mat4 model = glm::mat4(1.0f);
                model           = glm::rotate(model, glm::radians(155.0f), glm::vec3(0, 1, 0));
                model           = glm::translate(model, glm::vec3(-16, 17.5, -9));
                model           = glm::scale(model, glm::vec3(0.04));

                log_seat_shader->set_mat4("model", model);

                // Draw the campfire model.
                log_seat->draw(log_seat_shader);
            } else {
                glm::mat4 model = glm::mat4(1.0f);
                model           = glm::rotate(model, glm::radians(-100.0f), glm::vec3(0, 1, 0));
                model           = glm::translate(model, glm::vec3(1, 17.5, -26));
                model           = glm::scale(model, glm::vec3(0.04));

                log_seat_shader->set_mat4("model", model);

                // Draw the campfire model.
                log_seat->draw(log_seat_shader);
            }
        }
    }

    void MainController::draw_tents() {
        engine::resources::Model *viking_tent   = resources->model("viking_tent");
        engine::resources::Model *stylized_tent = resources->model("stylized_tent");
        engine::resources::Shader *tent_shader  = resources->shader("campfire_shader");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        // Activate the shader.
        tent_shader->use();

        // Set the light properties.
        tent_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            tent_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            tent_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            tent_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            tent_shader->set_float("material.shininess", 32.0f);
        } else {
            tent_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            tent_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            tent_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            tent_shader->set_float("material.shininess", 128.0f);
        }
        tent_shader->set_vec3("viewPos", camera->Position);

        // Set the projection and view matrices.
        tent_shader->set_mat4("projection", graphics->projection_matrix());
        tent_shader->set_mat4("view", camera->view_matrix());

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::rotate(model, glm::radians(-20.0f), glm::vec3(0, 1, 0));
        model           = glm::translate(model, glm::vec3(16, 17, -14));
        model           = glm::scale(model, glm::vec3(0.037));
        tent_shader->set_mat4("model", model);
        viking_tent->draw(tent_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-128.0f), glm::vec3(0, 1, 0));
        model = glm::translate(model, glm::vec3(0, 20, -33));
        model = glm::scale(model, glm::vec3(0.06));
        tent_shader->set_mat4("model", model);
        stylized_tent->draw(tent_shader);
    }

    void MainController::draw_old_tree() {
        engine::resources::Model *old_tree         = resources->model("old_tree");
        engine::resources::Shader *old_tree_shader = resources->shader("tree_shader2");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        // Activate the shader.
        old_tree_shader->use();

        // Set the light properties.
        old_tree_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            old_tree_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            old_tree_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            old_tree_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            old_tree_shader->set_float("material.shininess", 32.0f);
        } else {
            old_tree_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            old_tree_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            old_tree_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            old_tree_shader->set_float("material.shininess", 128.0f);
        }
        old_tree_shader->set_vec3("viewPos", camera->Position);

        // Set the projection and view matrices.
        old_tree_shader->set_mat4("projection", graphics->projection_matrix());
        old_tree_shader->set_mat4("view", camera->view_matrix());

        // Set the model matrix.
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::rotate(model, glm::radians(3.0f), glm::vec3(0, 0, 1));
        model           = glm::translate(model, glm::vec3(65, 40, -39));
        model           = glm::scale(model, glm::vec3(0.04));

        old_tree_shader->set_mat4("model", model);

        // Draw the campfire model.
        old_tree->draw(old_tree_shader);
    }

    void MainController::draw_bushes() {
        // Retrieve models and shader.
        auto bush1      = resources->model("bush1");
        auto bush2      = resources->model("bush2");
        auto laurelBush = resources->model("laurel_bush");
        auto bushShader = resources->shader("tree_shader2");

        // Set up light properties.
        glm::vec3 lightPos = is_day
                                 ? glm::vec3(0.0f, 60.0f, 0.0f)
                                 : glm::vec3(12.0f, 25.0f, 6.0f);
        bushShader->use();
        bushShader->set_vec3("light.position", lightPos);
        if (is_day) {
            bushShader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            bushShader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            bushShader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            bushShader->set_float("material.shininess", 32.0f);
        } else {
            bushShader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            bushShader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            bushShader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            bushShader->set_float("material.shininess", 128.0f);
        }
        bushShader->set_vec3("viewPos", camera->Position);
        bushShader->set_mat4("projection", graphics->projection_matrix());
        bushShader->set_mat4("view", camera->view_matrix());

        // Helper to set transformation and draw a model.
        auto drawModel = [bushShader](engine::resources::Model *model, const glm::mat4 &transform) {
            bushShader->set_mat4("model", transform);
            model->draw(bushShader);
        };

        // Lambda for drawing bush1 models (with a fixed rotation).
        auto drawBush1 = [&](const glm::vec3 &translation, float scale) {
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
            model           = glm::translate(model, translation);
            model           = glm::scale(model, glm::vec3(scale));
            drawModel(bush1, model);
        };

        // Lambda for drawing bush2 and laurel bush models.
        auto drawSimple = [&](engine::resources::Model *model, const glm::vec3 &translation, float scale) {
            glm::mat4 m = glm::mat4(1.0f);
            m           = glm::translate(m, translation);
            m           = glm::scale(m, glm::vec3(scale));
            drawModel(model, m);
        };

        // Draw bush1 instances.
        drawBush1(glm::vec3(-19, -3, 16), 5.0f);
        drawBush1(glm::vec3(15, 25, 16), 5.0f);
        drawBush1(glm::vec3(52, -19, 17), 5.0f);
        drawBush1(glm::vec3(31, -32, 17), 5.0f);
        drawBush1(glm::vec3(12, -24, 17), 5.0f);

        // Draw bush2 instances.
        drawSimple(bush2, glm::vec3(4, 20, -13), 0.3f);
        drawSimple(bush2, glm::vec3(32, 20, 4), 0.3f);
        drawSimple(bush2, glm::vec3(30, 20, 12), 0.3f);

        // Draw laurel bush instances.
        drawSimple(laurelBush, glm::vec3(-25, 16, 0), 0.680f);
        drawSimple(laurelBush, glm::vec3(-25, 16, 12), 0.680f);
        drawSimple(laurelBush, glm::vec3(-20, 16, 23), 0.680f);
        drawSimple(laurelBush, glm::vec3(-5, 16, 23), 0.680f);
        drawSimple(laurelBush, glm::vec3(6, 17, 20), 0.680f);
        drawSimple(laurelBush, glm::vec3(33, 17, -6), 0.680f);
    }

    void MainController::draw_white_flowers() {
        engine::resources::Model *white_flowers  = resources->model("flowers2");
        engine::resources::Shader *flower_shader = resources->shader("flower_shader");

        // Define three rows with 10 columns each.
        unsigned int rowCount    = 2;
        unsigned int colCount    = 10;
        unsigned int amount      = rowCount * colCount + 13;
        glm::mat4 *modelMatrices = new glm::mat4[amount];

        // Loop over each row and column.
        for (unsigned int row = 0; row < rowCount; row++) {
            // Choose the x coordinate based on the row.
            float x = 0.0f;
            if (row == 0)
                x = 40.0f;
            else if (row == 1)
                x = 44.0f;
            else if (row == 2)
                x = 48.0f;

            for (unsigned int col = 0; col < colCount; col++) {
                unsigned int index   = row * colCount + col;
                glm::mat4 model      = glm::mat4(1.0f);
                model                = glm::translate(model, glm::vec3(x, 17.4f, 4.0f * col - 16));
                model                = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
                model                = glm::scale(model, glm::vec3(0.12f));
                modelMatrices[index] = model;
            }
        }
        std::vector<glm::vec3> translations = {
                glm::vec3(-3, -21, -17.2),
                glm::vec3(-7, -17, -17.2),
                glm::vec3(-14, -11, -17.2),
                glm::vec3(-22, -4, -17.2),
                glm::vec3(-27, 5, -15),
                glm::vec3(-15, 28, -17.2),
                glm::vec3(-1, 34, -17.2),
                glm::vec3(9, 36, -17.2),
                glm::vec3(18, 36, -17.2),
                glm::vec3(28, 36, -17.2),
                glm::vec3(38, 29, -17.2),
                glm::vec3(51, 9, -17.2),
                glm::vec3(5, -16, -17.2)
        };

        for (unsigned int i = rowCount * colCount; i < amount; i++) {
            auto model       = glm::mat4(1.0f);
            model            = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
            model            = glm::translate(model, translations[i - rowCount * colCount]);
            model            = glm::scale(model, glm::vec3(0.12f));
            modelMatrices[i] = model;
        }

        // Create and fill the instance VBO.
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), modelMatrices, GL_STATIC_DRAW);

        // For each mesh in the model, bind the instance matrix buffer to its VAO.
        for (unsigned int i = 0; i < white_flowers->meshes().size(); i++) {
            unsigned int VAO = white_flowers->meshes()[i].m_vao;
            glBindVertexArray(VAO);

            std::size_t vec4Size = sizeof(glm::vec4);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) 0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (vec4Size));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (2 * vec4Size));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (3 * vec4Size));

            // Mark these attributes as instanced.
            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }

        // Set shader uniforms...
        glm::vec3 lightPos = is_day
                                 ? glm::vec3(0.0f, 60.0f, 0.0f)
                                 : glm::vec3(12.0f, 25.0f, 6.0f);
        flower_shader->use();
        flower_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            flower_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            flower_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            flower_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            flower_shader->set_float("material.shininess", 32.0f);
        } else {
            flower_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            flower_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            flower_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            flower_shader->set_float("material.shininess", 128.0f);
        }
        flower_shader->set_vec3("viewPos", camera->Position);
        flower_shader->set_mat4("projection", graphics->projection_matrix());
        flower_shader->set_mat4("view", camera->view_matrix());

        // Draw using instancing.
        white_flowers->drawInstanced(flower_shader, amount);

        delete[] modelMatrices;
    }

    void MainController::draw_flowers() {
        draw_white_flowers();
        draw_red_flowers();
    }

    void MainController::draw_path() {
        engine::resources::Model *path          = resources->model("path");
        engine::resources::Shader *stone_shader = resources->shader("campfire_shader");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        stone_shader->use();

        stone_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            stone_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            stone_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            stone_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            stone_shader->set_float("material.shininess", 32.0f);
        } else {
            stone_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            stone_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            stone_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            stone_shader->set_float("material.shininess", 128.0f);
        }
        stone_shader->set_vec3("viewPos", camera->Position);
        stone_shader->set_mat4("projection", graphics->projection_matrix());
        stone_shader->set_mat4("view", camera->view_matrix());

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
        model           = glm::rotate(model, glm::radians(10.0f), glm::vec3(0, 1, 0));
        model           = glm::rotate(model, glm::radians(15.0f), glm::vec3(0, 0, 1));
        model           = glm::translate(model, glm::vec3(-13, 22, -20));
        model           = glm::scale(model, glm::vec3(0.19f));
        stone_shader->set_mat4("model", model);
        path->draw(stone_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(-1.0f), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0, 0, 1));
        model = glm::translate(model, glm::vec3(-11, 19, -17));
        model = glm::scale(model, glm::vec3(0.19f));
        stone_shader->set_mat4("model", model);
        path->draw(stone_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0, 0, 1));
        model = glm::translate(model, glm::vec3(-6.5, 15, -17.5));
        model = glm::scale(model, glm::vec3(0.19f));
        stone_shader->set_mat4("model", model);
        path->draw(stone_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0, 0, 1));
        model = glm::translate(model, glm::vec3(-1, 12, -17.5));
        model = glm::scale(model, glm::vec3(0.19f));
        stone_shader->set_mat4("model", model);
        path->draw(stone_shader);
    }

    void MainController::draw_mushrooms() {
        engine::resources::Model *mushroom       = resources->model("shrooms");
        engine::resources::Shader *shroom_shader = resources->shader("campfire_shader");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        shroom_shader->use();

        shroom_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            shroom_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            shroom_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            shroom_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            shroom_shader->set_float("material.shininess", 32.0f);
        } else {
            shroom_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            shroom_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            shroom_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            shroom_shader->set_float("material.shininess", 128.0f);
        }
        shroom_shader->set_vec3("viewPos", camera->Position);
        shroom_shader->set_mat4("projection", graphics->projection_matrix());
        shroom_shader->set_mat4("view", camera->view_matrix());

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model           = glm::rotate(model, glm::radians(-19.0f), glm::vec3(0, 1, 0));
        model           = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 0, 1));
        model           = glm::translate(model, glm::vec3(6, 0, 16));
        model           = glm::scale(model, glm::vec3(0.19f));
        shroom_shader->set_mat4("model", model);
        mushroom->draw(shroom_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(3, 8, 17));
        model = glm::scale(model, glm::vec3(0.19f));
        shroom_shader->set_mat4("model", model);
        mushroom->draw(shroom_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(12, 19, 17));
        model = glm::scale(model, glm::vec3(0.19f));
        shroom_shader->set_mat4("model", model);
        mushroom->draw(shroom_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(30, 1, 17));
        model = glm::scale(model, glm::vec3(0.19f));
        shroom_shader->set_mat4("model", model);
        mushroom->draw(shroom_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(30, -10, 17));
        model = glm::scale(model, glm::vec3(0.19f));
        shroom_shader->set_mat4("model", model);
        mushroom->draw(shroom_shader);
    }

    void MainController::draw_red_flowers() {
        engine::resources::Model *orange_flower  = resources->model("orange_flower");
        engine::resources::Model *roses          = resources->model("roses");
        engine::resources::Shader *flower_shader = resources->shader("flower_shader");

        unsigned int amount                 = 21;
        glm::mat4 *modelMatrices            = new glm::mat4[amount];
        std::vector<glm::vec3> translations = {
                glm::vec3(-10, 0, 17),
                glm::vec3(-4, 9, 17),
                glm::vec3(-4, 19, 17),
                glm::vec3(-12, 13, 17),
                glm::vec3(-15, 6, 17),
                glm::vec3(-17, 3, 17),
                glm::vec3(34, 2, 17),
                glm::vec3(43, 2, 17),
                glm::vec3(43, 10, 17),
                glm::vec3(31, 10, 17),
                glm::vec3(31, 15, 17),
                glm::vec3(49, -3, 17),
                glm::vec3(50, -14, 17),
                glm::vec3(43, -26, 17),
                glm::vec3(26, -29, 17),
                glm::vec3(17, -29, 17),
                glm::vec3(17, -40, 17),
                glm::vec3(10, -37, 17),
                glm::vec3(4, -38, 17),
                glm::vec3(4, -23, 17),
                glm::vec3(7, -23, 17)
        };

        for (int i = 0; i < amount; i++) {
            auto model       = glm::mat4(1.0f);
            model            = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
            model            = glm::translate(model, translations[i]);
            model            = glm::scale(model, glm::vec3(0.04f));
            modelMatrices[i] = model;
        }

        // Create and fill the instance VBO.
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), modelMatrices, GL_STATIC_DRAW);

        // For each mesh in the model, bind the instance matrix buffer to its VAO.
        for (unsigned int i = 0; i < roses->meshes().size(); i++) {
            unsigned int VAO = roses->meshes()[i].m_vao;
            glBindVertexArray(VAO);

            std::size_t vec4Size = sizeof(glm::vec4);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) 0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (vec4Size));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (2 * vec4Size));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (3 * vec4Size));

            // Mark these attributes as instanced.
            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }

        // Set shader uniforms...
        glm::vec3 lightPos = is_day
                                 ? glm::vec3(0.0f, 60.0f, 0.0f)
                                 : glm::vec3(12.0f, 25.0f, 6.0f);
        flower_shader->use();
        flower_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            flower_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            flower_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            flower_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            flower_shader->set_float("material.shininess", 32.0f);
        } else {
            flower_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            flower_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            flower_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            flower_shader->set_float("material.shininess", 128.0f);
        }
        flower_shader->set_vec3("viewPos", camera->Position);
        flower_shader->set_mat4("projection", graphics->projection_matrix());
        flower_shader->set_mat4("view", camera->view_matrix());

        // Draw using instancing.
        roses->drawInstanced(flower_shader, amount);

        delete[] modelMatrices;
    }

    void MainController::draw_terrain() {
        engine::resources::Model *terrain         = resources->model("terrain");
        engine::resources::Shader *terrain_shader = resources->shader("terrain_shader");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        terrain_shader->use();
        terrain_shader->set_vec3("light.position", lightPos);

        if (is_day) {
            terrain_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            terrain_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        } else {
            terrain_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            terrain_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        }

        // Set projection and view matrices.
        terrain_shader->set_mat4("projection", graphics->projection_matrix());
        terrain_shader->set_mat4("view", camera->view_matrix());

        // Set model transform.
        glm::mat4 model = glm::mat4(1.0f);
        terrain_shader->set_mat4("model", model);

        // Draw the terrain model.
        terrain->draw(terrain_shader);
    }

    void MainController::test() {
        engine::resources::Model *test_model   = resources->model("pine_tree");
        engine::resources::Shader *test_shader = resources->shader("tree_shader2");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        // Activate the shader.
        test_shader->use();

        // Set the light properties.
        test_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            test_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            test_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            test_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            test_shader->set_float("material.shininess", 32.0f);
        } else {
            test_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            test_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            test_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            test_shader->set_float("material.shininess", 128.0f);
        }
        test_shader->set_vec3("viewPos", camera->Position);

        // Set the projection and view matrices.
        test_shader->set_mat4("projection", graphics->projection_matrix());
        test_shader->set_mat4("view", camera->view_matrix());

        // Set the model matrix.
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::rotate(model, glm::radians(test_rotation_x), glm::vec3(1, 0, 0));
        model           = glm::rotate(model, glm::radians(test_rotation_y), glm::vec3(0, 1, 0));
        model           = glm::rotate(model, glm::radians(test_rotation_z), glm::vec3(0, 0, 1));
        model           = glm::translate(model, glm::vec3(test_x, test_y, test_z));
        model           = glm::scale(model, glm::vec3(test_scale));

        test_shader->set_mat4("model", model);

        // Draw the campfire model.
        test_model->draw(test_shader);
    }

    void MainController::draw_skybox() {
        auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
        engine::resources::Skybox *skybox_cube;
        if (is_day)
            skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox_day");
        else
            skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox(
                    "skybox_night");
        engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
    }

    void MainController::drawLightSource_day() {
        if (!is_day)
            return;

        auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                     = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *sun     = resources->model("sun");
        engine::resources::Shader *shader = resources->shader("daylight_shader");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, -20.0f, 0.0f));
        model           = glm::scale(model, glm::vec3(1.0f));
        shader->set_mat4("model", model);
        sun->draw(shader);
    }

    void MainController::update() {
        update_camera();
    }

    void MainController::update_camera() {
        auto gui = engine::core::Controller::get<GUIController>();
        if (gui->is_enabled())
            return;

        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto camera   = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        float dt      = platform->dt();
        if (platform->key(engine::platform::KeyId::KEY_W).is_down() || platform->key(engine::platform::KeyId::KEY_UP).
            is_down()) {
            if (platform->key(engine::platform::KEY_LEFT_SHIFT).is_down())
                camera->MovementSpeed = 20;
            else
                camera->MovementSpeed = 7;
            camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_S).is_down() || platform->key(engine::platform::KeyId::KEY_DOWN).
            is_down()) {
            if (platform->key(engine::platform::KEY_LEFT_SHIFT).is_down())
                camera->MovementSpeed = 20;
            else
                camera->MovementSpeed = 7;
            camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_A).is_down() || platform->key(engine::platform::KeyId::KEY_LEFT).
            is_down()) {
            if (platform->key(engine::platform::KEY_LEFT_SHIFT).is_down())
                camera->MovementSpeed = 20;
            else
                camera->MovementSpeed = 7;
            camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_D).is_down() || platform->key(engine::platform::KeyId::KEY_RIGHT)
                                                                               .is_down()) {
            if (platform->key(engine::platform::KEY_LEFT_SHIFT).is_down())
                camera->MovementSpeed = 20;
            else
                camera->MovementSpeed = 7;
            camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_SPACE).is_down()) {
            if (platform->key(engine::platform::KEY_LEFT_SHIFT).is_down())
                camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt);
            else
                camera->move_camera(engine::graphics::Camera::Movement::UP, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_P).state() == engine::platform::Key::State::JustPressed) {
            mouse_enabled = !mouse_enabled;
            platform->set_enable_cursor(mouse_enabled);
        }
        if (platform->key(engine::platform::KeyId::KEY_N).state() == engine::platform::Key::State::JustPressed) {
            is_day = !is_day;
        }
        if (platform->key(engine::platform::KeyId::KEY_Q).is_down()) {
            camera->rotate_camera(5, 0);
        }
        if (platform->key(engine::platform::KeyId::KEY_E).is_down()) {
            camera->rotate_camera(5, 0);
        }
    }
}
