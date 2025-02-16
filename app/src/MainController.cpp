#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <MainController.hpp>
#include <GUIController.hpp>

namespace app {

    float test_scale    = 1.0f;
    float test_rotation = 0.0f;
    float test_x        = 0.0f;
    float test_y        = 0.0f;
    float test_z        = 0.0f;

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
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
        mouse_enabled = false;
        platform->set_enable_cursor(mouse_enabled);
        is_day = true;
        test_x = test_y = test_z = test_rotation = test_scale = 0.0f;
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
        // draw_forest();
        // drawLightSource_day();
        test();
        draw_skybox();
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
        engine::resources::Shader *test_shader = resources->shader("campfire_shader");

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

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::rotate(model, glm::radians(-20.0f), glm::vec3(0, 1, 0));
        model           = glm::translate(model, glm::vec3(16, 17, -14));
        model           = glm::scale(model, glm::vec3(0.037));
        test_shader->set_mat4("model", model);
        viking_tent->draw(test_shader);

        model = glm::mat4(1.0f);
        model           = glm::rotate(model, glm::radians(-128.0f), glm::vec3(0, 1, 0));
        model           = glm::translate(model, glm::vec3(0, 20, -33));
        model           = glm::scale(model, glm::vec3(0.06));
        test_shader->set_mat4("model", model);
        stylized_tent->draw(test_shader);
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
        engine::resources::Model *test_model = resources->model("test");
        engine::resources::Shader *test_shader = resources->shader("campfire_shader");

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
        model           = glm::rotate(model, glm::radians(test_rotation), glm::vec3(0, 1, 0));
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
