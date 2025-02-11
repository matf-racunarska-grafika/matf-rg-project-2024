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

    bool mouse_enabled;
    bool is_day;

    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
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
        // test();
        draw_terrain();
        draw_campfire();
        // draw_logs();
        // draw_tents();
        // draw_forest();
        // drawLightSource_day();
        // draw_island();
        draw_skybox();
    }




    void MainController::draw_campfire() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        engine::resources::Model *campfire = resources->model("campfire");
        engine::resources::Shader *campfire_shader = resources->shader("campfire_shader");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 30.0f, 0.0f) : glm::vec3(0.0f, 3.0f, 0.0f);

        if (is_day) {
            campfire_shader->use();

            campfire_shader->set_vec3("light.position", lightPos);
            campfire_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            campfire_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            campfire_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            campfire_shader->set_vec3("viewPos", camera->Position);

            campfire_shader->set_vec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f)); // specular highlights
            campfire_shader->set_float("material.shininess", 32.0f);

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
            campfire_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            campfire_shader->set_vec3("viewPos", camera->Position);

            campfire_shader->set_vec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f)); // specular highlights
            campfire_shader->set_float("material.shininess", 128.0f);

            campfire_shader->set_mat4("projection", graphics->projection_matrix());
            campfire_shader->set_mat4("view", camera->view_matrix());

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.1));
            model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
            campfire_shader->set_mat4("model", model);

            campfire->draw(campfire_shader);
        }
    }

    void MainController::draw_terrain() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        engine::resources::Model *campfire = resources->model("terrain");
        engine::resources::Shader *campfire_shader = resources->shader("campfire_shader");

        glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 60.0f, 0.0f) : glm::vec3(0.0f, 3.0f, 0.0f);

        if (is_day) {
            campfire_shader->use();

            campfire_shader->set_vec3("light.position", lightPos);
            campfire_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            campfire_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            campfire_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            campfire_shader->set_vec3("viewPos", camera->Position);

            campfire_shader->set_vec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f)); // specular highlights
            campfire_shader->set_float("material.shininess", 32.0f);

            campfire_shader->set_mat4("projection", graphics->projection_matrix());
            campfire_shader->set_mat4("view", camera->view_matrix());

            glm::mat4 model = glm::mat4(1.0f);
            // model = glm::scale(model, glm::vec3());
            // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0, 0));
            // model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
            campfire_shader->set_mat4("model", model);

            campfire->draw(campfire_shader);
        } else {
            campfire_shader->use();

            campfire_shader->set_vec3("light.position", lightPos);
            campfire_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            campfire_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            campfire_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            campfire_shader->set_vec3("viewPos", camera->Position);

            campfire_shader->set_vec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f)); // specular highlights
            campfire_shader->set_float("material.shininess", 128.0f);

            campfire_shader->set_mat4("projection", graphics->projection_matrix());
            campfire_shader->set_mat4("view", camera->view_matrix());

            glm::mat4 model = glm::mat4(1.0f);
            // model = glm::scale(model, glm::vec3(0.1));
            // model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
            campfire_shader->set_mat4("model", model);

            campfire->draw(campfire_shader);
        }
    }


    void MainController::test() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    engine::resources::Model* campfire = resources->model("campfire");
    engine::resources::Shader* campfire_shader = resources->shader("test");

    // Determine light position based on time of day.
    glm::vec3 lightPos = is_day ? glm::vec3(0.0f, 30.0f, 0.0f)
                                : glm::vec3(0.0f, 3.0f, 0.0f);

    // Activate the shader.
    campfire_shader->use();

    // Set the light properties.
    campfire_shader->set_vec3("light.position", lightPos);
    if (is_day) {
        campfire_shader->set_vec3("light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
        campfire_shader->set_vec3("light.diffuse",  glm::vec3(0.5f, 0.5f, 0.5f));
        campfire_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        campfire_shader->set_float("material.shininess", 32.0f);
    } else {
        campfire_shader->set_vec3("light.ambient",  glm::vec3(0.1f, 0.1f, 0.1f));
        campfire_shader->set_vec3("light.diffuse",  glm::vec3(0.3f, 0.3f, 0.3f));
        campfire_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        campfire_shader->set_float("material.shininess", 128.0f);
    }
    campfire_shader->set_vec3("viewPos", camera->Position);

    // // Bind texture units.
    // // Assume diffuse map -> texture unit 0, specular map -> unit 1, normal map -> unit 2.
    // campfire_shader->set_int("texture_diffuse1", 0);
    // campfire_shader->set_int("texture_specular", 1);
    // campfire_shader->set_int("texture_normal", 2);

    // Set the projection and view matrices.
    campfire_shader->set_mat4("projection", graphics->projection_matrix());
    campfire_shader->set_mat4("view", camera->view_matrix());

    // Set the model matrix.
    glm::mat4 model = glm::mat4(1.0f);
    campfire_shader->set_mat4("model", model);

    // Draw the campfire model.
    campfire->draw(campfire_shader);
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

    void MainController::draw_island() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera    = graphics->camera();

        engine::resources::Model *island         = resources->model("nature");
        engine::resources::Shader *island_shader = resources->shader("test");

        glm::vec3 lightPos = is_day
                                 ? glm::vec3(0.0f, 30.0f, 0.0f)
                                 : glm::vec3(0.0f, 3.0f, 0.0f);

        island_shader->use();

        island_shader->set_vec3("light.position", lightPos);
        if (is_day) {
            island_shader->set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            island_shader->set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
            island_shader->set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            island_shader->set_float("material.shininess", 32.0f);
        } else {
            island_shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            island_shader->set_vec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
            island_shader->set_vec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            island_shader->set_float("material.shininess", 128.0f);
        }

        island_shader->set_vec3("viewPos", camera->Position);

        island_shader->set_vec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f)); // white; adjust as needed

        island_shader->set_mat4("projection", graphics->projection_matrix());
        island_shader->set_mat4("view", camera->view_matrix());

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::scale(model, glm::vec3(3.0, 3.5, 3.0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        // model = glm::translate(model, glm::vec3(0, -, 0));
        island_shader->set_mat4("model", model);

        island->draw(island_shader);
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
        if (platform->key(engine::platform::KeyId::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
            mouse_enabled = !mouse_enabled;
            platform->set_enable_cursor(mouse_enabled);
        }
        if (platform->key(engine::platform::KeyId::KEY_N).state() == engine::platform::Key::State::JustPressed) {
            is_day = !is_day;
        }
    }
}
