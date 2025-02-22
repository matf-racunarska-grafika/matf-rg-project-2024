#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <MainController.hpp>
#include <GUIController.hpp>

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
        auto camera   = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
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
        test();
        draw_skybox();
    }

    void MainController::draw_forest() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto gui = engine::core::Controller::get<GUIController>();
        auto camera = graphics->camera();
        engine::resources::Model *yellow_tree = resources->model("yellow_tree");
        engine::resources::Model *green_tree = resources->model("green_tree");
        engine::resources::Model *tall_tree = resources->model("beech_tree");
        engine::resources::Model *oak_tree = resources->model("oak_tree");
        engine::resources::Model *tree_gate = resources->model("tree_gate");
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
    }

    void MainController::draw_campfire() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        engine::resources::Model *campfire = resources->model("campfire");
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
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto gui = engine::core::Controller::get<GUIController>();
        auto camera = graphics->camera();
        engine::resources::Model *log_seat = resources->model("log_seat");
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
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto gui = engine::core::Controller::get<GUIController>();
        auto camera = graphics->camera();
        engine::resources::Model *viking_tent = resources->model("viking_tent");
        engine::resources::Model *stylized_tent = resources->model("stylized_tent");
        engine::resources::Shader *tent_shader = resources->shader("campfire_shader");

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
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto gui = engine::core::Controller::get<GUIController>();
        auto camera = graphics->camera();
        engine::resources::Model *old_tree = resources->model("old_tree");
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
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto gui = engine::core::Controller::get<GUIController>();
        auto camera = graphics->camera();
        engine::resources::Model *bush1 = resources->model("bush1");
        engine::resources::Model *bush2 = resources->model("bush2");
        engine::resources::Shader *bush_shader = resources->shader("tree_shader2");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        // Activate the shader.
        bush_shader->use();

        // Set the light properties.
        bush_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            bush_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            bush_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            bush_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            bush_shader->set_float("material.shininess", 32.0f);
        } else {
            bush_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            bush_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            bush_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            bush_shader->set_float("material.shininess", 128.0f);
        }
        bush_shader->set_vec3("viewPos", camera->Position);

        bush_shader->set_mat4("projection", graphics->projection_matrix());
        bush_shader->set_mat4("view", camera->view_matrix());

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model           = glm::translate(model, glm::vec3(-19, -3, 16));
        model           = glm::scale(model, glm::vec3(5.0f));
        bush_shader->set_mat4("model", model);
        bush1->draw(bush_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(15, 25, 16));
        model = glm::scale(model, glm::vec3(5.0f));
        bush_shader->set_mat4("model", model);
        bush1->draw(bush_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(52, -19, 17));
        model = glm::scale(model, glm::vec3(5.0f));
        bush_shader->set_mat4("model", model);
        bush1->draw(bush_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(31, -32, 17));
        model = glm::scale(model, glm::vec3(5.0f));
        bush_shader->set_mat4("model", model);
        bush1->draw(bush_shader);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(12, -24, 17));
        model = glm::scale(model, glm::vec3(5.0f));
        bush_shader->set_mat4("model", model);
        bush1->draw(bush_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4, 20, -13));
        model = glm::scale(model, glm::vec3(0.3f));
        bush_shader->set_mat4("model", model);
        bush2->draw(bush_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(32, 20, 4));
        model = glm::scale(model, glm::vec3(0.3f));
        bush_shader->set_mat4("model", model);
        bush2->draw(bush_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(30, 20, 12));
        model = glm::scale(model, glm::vec3(0.3f));
        bush_shader->set_mat4("model", model);
        bush2->draw(bush_shader);
    }

    void MainController::draw_flowers() {
        auto resources     = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics      = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera        = graphics->camera();
        auto flower_shader = resources->shader("tree_shader2");
        auto plant_shader  = resources->shader("plant_shader");
        auto plants        = resources->model("plants");
        auto orange_flower = resources->model("orange_flower");

        // Activate the shader and set up common shader parameters.
        flower_shader->use();
        flower_shader->set_vec3("viewPos", camera->Position);
        flower_shader->set_mat4("projection", graphics->projection_matrix());
        flower_shader->set_mat4("view", camera->view_matrix());

        std::vector<glm::mat4> positions_orange;
        auto model = glm::mat4(1.0f);
        model      = glm::translate(model, glm::vec3(-5, 17.2, 0));
        model      = glm::scale(model, glm::vec3(0.02f));
        positions_orange.push_back(model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(8, 17.2, -6));
        model = glm::scale(model, glm::vec3(0.02f));
        positions_orange.push_back(model);

        for (glm::mat4 pos: positions_orange) {
            flower_shader->set_mat4("model", pos);
            orange_flower->draw(flower_shader);
        }

        glm::vec3 offsets[] = {
            glm::vec3(32, 18, 0),
            glm::vec3(32, 18, 4),
            glm::vec3(32, 18, 8),
            glm::vec3(32, 18, 12),
            glm::vec3(32, 18, 16),
            glm::vec3(32, 18, 20),
            glm::vec3(32, 18, 24),
            glm::vec3(32, 18, 28),
            glm::vec3(36, 18, 0),
            glm::vec3(36, 18, 4),
            glm::vec3(36, 18, 8),
            glm::vec3(36, 18, 12),
            glm::vec3(36, 18, 16),
            glm::vec3(36, 18, 20),
            glm::vec3(36, 18, 24),
            glm::vec3(36, 18, 28),
            glm::vec3(40, 18, 0),
            glm::vec3(40, 18, 4),
            glm::vec3(40, 18, 8),
            glm::vec3(40, 18, 12),
            glm::vec3(40, 18, 16),
            glm::vec3(40, 18, 20),
            glm::vec3(40, 18, 24),
            glm::vec3(40, 18, 28),
            glm::vec3(44, 18, 0),
            glm::vec3(44, 18, 4),
            glm::vec3(44, 18, 8),
            glm::vec3(44, 18, 12),
            glm::vec3(44, 18, 16),
            glm::vec3(44, 18, 20),
            glm::vec3(44, 18, 24),
            glm::vec3(44, 18, 28),
            glm::vec3(48, 18, 0),
            glm::vec3(48, 18, 4),
            glm::vec3(48, 18, 8),
            glm::vec3(48, 18, 12),
            glm::vec3(48, 18, 16),
            glm::vec3(48, 18, 20),
            glm::vec3(48, 18, 24),
            glm::vec3(48, 18, 28),
            glm::vec3(52, 18, 0),
            glm::vec3(52, 18, 4),
            glm::vec3(52, 18, 8),
            glm::vec3(52, 18, 12),
            glm::vec3(52, 18, 16),
            glm::vec3(52, 18, 20),
            glm::vec3(52, 18, 24),
            glm::vec3(52, 18, 28)
        };

        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.01f));

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(12.0f, 25.0f, 6.0f);

        plant_shader->use();
        plant_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            plant_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            plant_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            plant_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            plant_shader->set_float("material.shininess", 32.0f);
        } else {
            plant_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            plant_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            plant_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            plant_shader->set_float("material.shininess", 128.0f);
        }
        plant_shader->set_vec3("viewPos", camera->Position);
        plant_shader->set_mat4("projection", graphics->projection_matrix());
        plant_shader->set_mat4("view", camera->view_matrix());

        for (unsigned int i = 0; i < 48; i++) {
            plant_shader->set_mat4("model", model);
            plant_shader->set_vec3("offsets[" + std::to_string(i) + "]", offsets[i]);
        }

        plants->drawInstanced(plant_shader, 48);
    }

    void MainController::draw_terrain() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        engine::resources::Model *terrain = resources->model("terrain");
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
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto gui = engine::core::Controller::get<GUIController>();
        auto camera = graphics->camera();
        engine::resources::Model *test_model = resources->model("plants");
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
    }
}
