#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/graphics/PostProcessingController.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <spdlog/spdlog.h>

#include <GuiController.hpp>
#include <MainController.hpp>
#include <ProgramState.hpp>

namespace app {
    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto gui_controller = engine::core::Controller::get<GuiController>();
        if (!gui_controller->is_enabled()) {
            auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
            camera->rotate_camera(position.dx, position.dy);
        }
    }

    void MainController::initialize() {
        spdlog::info("MainController initialized");
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

    void MainController::draw_skull() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        float time    = platform->frame_time().current;
        // Model
        auto resources                        = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                         = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *skull_model = resources->model("skull");

        // Shader
        engine::resources::Shader *shader = resources->shader("NearShader");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, 0.5f, -46.0f));
        model           = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

        // Speed of the skull rotation
        float skull_speed = Settings::get_instance().skull_speed;
        model             = glm::rotate(model, glm::radians(time * skull_speed), glm::vec3(0.0f, 0.0f, 1.0f));
        model             = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader->set_mat4("model", model);

        shader->set_vec3("viewPosition", graphics->camera()->Position);
        shader->set_float("material.shininess", 32.0f);

        shader->set_vec3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -0.7f));
        shader->set_vec3("dirLight.ambient", glm::vec3(0.025f, 0.005f, 0.005f));
        shader->set_vec3("dirLight.diffuse", glm::vec3(0.025f, 0.005f, 0.005f));
        shader->set_vec3("dirLight.specular", glm::vec3(0.025f, 0.005f, 0.005f));

        // getting the possitions of the eyes
        glm::mat4 skull_eye_1 = model;
        glm::mat4 skull_eye_2 = model;

        skull_eye_1 = glm::translate(skull_eye_1, glm::vec3(-4.4f, -9.8f, 14.0f));
        skull_eye_1 = glm::scale(skull_eye_1, glm::vec3(0.008f));

        skull_eye_2 = glm::translate(skull_eye_2, glm::vec3(4.4f, -9.8f, 14.0f));
        skull_eye_2 = glm::scale(skull_eye_2, glm::vec3(0.008f));

        glm::vec4 eye_1_pos = skull_eye_1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec4 eye_2_pos = skull_eye_2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        glm::vec3 light_color = Settings::get_instance().light_color;
        shader->set_vec3("pointLights[0].position", glm::vec3(eye_1_pos));
        shader->set_vec3("pointLights[0].ambient", glm::vec3(0.0f));
        shader->set_vec3("pointLights[0].diffuse", light_color);
        shader->set_vec3("pointLights[0].specular", light_color);
        shader->set_float("pointLights[0].constant", 1.0f);
        shader->set_float("pointLights[0].linear", 0.9f);
        shader->set_float("pointLights[0].quadratic", 0.92f);

        shader->set_vec3("pointLights[1].position", glm::vec3(eye_2_pos));
        shader->set_vec3("pointLights[1].ambient", glm::vec3(0.0f));
        shader->set_vec3("pointLights[1].diffuse", light_color);
        shader->set_vec3("pointLights[1].specular", light_color);
        shader->set_float("pointLights[1].constant", 1.0f);
        shader->set_float("pointLights[1].linear", 0.9f);
        shader->set_float("pointLights[1].quadratic", 0.92f);

        skull_model->draw(shader);

        draw_eyes(skull_eye_1, skull_eye_2);
    }

    void MainController::draw_bridge() {
        // Model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *bridge_model = resources->model("bridge");

        // Shader
        engine::resources::Shader *shader = resources->shader("NearShader");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("model", glm::mat4(1.0f));
        glm::mat4 model = glm::mat4(1.0f);
        for (int i = 0; i <= 7; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -2.0f, -i * 3.39f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f));
            shader->set_mat4("model", model);

            bridge_model->draw(shader);
        }
    }

    void MainController::draw_eyes(glm::mat4 eye_1_model, glm::mat4 eye_2_model) {
        // Model
        auto resources                      = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                       = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *eye_model = resources->model("eyes");

        // Shader
        engine::resources::Shader *shader = resources->shader("eyeShader");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_vec3("lightColor", Settings::get_instance().light_color);

        shader->set_mat4("model", eye_1_model);
        eye_model->draw(shader);
        shader->set_mat4("model", eye_2_model);
        eye_model->draw(shader);
    }

    void MainController::draw_arena() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        float time    = platform->frame_time().current;

        // Model
        auto resources                        = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                         = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *arena_model = resources->model("arena");

        // Shader
        engine::resources::Shader *shader = resources->shader("NearShader");
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, -20.0f, -10.0f));
        model           = glm::rotate(model, glm::radians(time), glm::vec3(0.0f, 1.0f, 0.0f));
        model           = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model           = glm::scale(model, glm::vec3(0.008f));
        shader->set_mat4("model", model);

        arena_model->draw(shader);
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox    = resources->skybox("skybox");
        auto shader    = resources->shader("skybox");
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->draw_skybox(shader, skybox);
    }

    void MainController::draw_health_bar() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Shader *heart_shader = resources->shader("heart");
        engine::resources::Texture *heart_texture = resources->texture("heart");

        engine::graphics::OpenGL::disable_depth_testing();
        engine::graphics::OpenGL::enable_blend();
        engine::graphics::OpenGL::gl_blend_func();

        heart_shader->use();
        engine::graphics::Framebuffer::activate_texture(heart_texture->id(), 0);

        auto window         = engine::core::Controller::get<engine::platform::PlatformController>()->window();
        float screen_width  = window->width();
        float screen_height = window->height();

        const float heart_size = 50.0f;
        const float padding    = 10.0f;
        const float startX     = screen_width - heart_size - padding;
        const float startY     = padding + 35.0f;

        int current_health = Settings::get_instance().health;
        for (int i = 0; i < current_health; ++i) {
            float x = startX - i * (heart_size + padding);
            float y = startY;

            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, glm::vec3(
                                           (x / screen_width) * 2.0f - 1.0f,
                                           1.0f - (y / screen_height) * 2.0f,
                                           0.0f
                                           ));
            model = glm::scale(model, glm::vec3(
                                       heart_size / screen_width,
                                       heart_size / screen_height,
                                       1.0f
                                       ));

            heart_shader->set_mat4("model", model);
            engine::graphics::Framebuffer::render_quad();
        }
        engine::graphics::OpenGL::disable_blend();
        engine::graphics::OpenGL::enable_depth_testing();
    }

    void MainController::draw() {
        draw_bridge();
        draw_skull();
        draw_arena();
        draw_skybox();

        draw_health_bar();
    }

    void MainController::end_draw() {
        engine::graphics::Framebuffer::bind_framebuffer(0);
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

    void set_camera_to_starting_position() {
        auto graphics    = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera      = graphics->camera();
        camera->Position = glm::vec3(0.0f);
        camera->Front    = glm::vec3(0.0f, 0.0f, -1.0f);
        camera->Up       = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    void MainController::update_camera() {
        auto gui_controller = engine::core::Controller::get<GuiController>();
        if (gui_controller->is_enabled()) {
            return;
        }
        auto platform    = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics    = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera      = graphics->camera();
        float dt         = platform->dt();
        bool skullFacing = Settings::get_instance().skull_facing_player;

        if (Settings::get_instance().spectator_mode) {
            float speed = 10.0f;
            if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
                camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt * speed);
            }
            if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
                camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt * speed);
            }
            if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
                camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt * speed);
            }
            if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
                camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt * speed);
            }
            if (platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).is_down()) {
                camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt * speed);
            }
            if (platform->key(engine::platform::KeyId::KEY_SPACE).is_down()) {
                camera->move_camera(engine::graphics::Camera::Movement::UP, dt * speed);
            }
        } else {
            if (platform->frame_time().current < Settings::get_instance().teleport_cooldown)
                return;
            if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
                if (!skullFacing) {
                    camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
                } else {
                    Settings::get_instance().health--;
                    // teleport player to start if he has been spotted
                    set_camera_to_starting_position();
                    Settings::get_instance().teleport_cooldown = platform->frame_time().current + 0.5f;
                }
            }
        }
    }

    void MainController::update() {
        update_camera();
        update_skull_facing();
        update_game_state();
    }

    void MainController::update_game_state() {
        int health = Settings::get_instance().health;
        if (health <= 0 && !Settings::get_instance().spectator_mode) {
            auto gui_controller = engine::core::Controller::get<GuiController>();
            auto graphics       = engine::core::Controller::get<engine::graphics::GraphicsController>();
            auto camera         = graphics->camera();
            gui_controller->set_enable(true);
            Settings::get_instance().health = Settings::get_instance().max_health;
            set_camera_to_starting_position();
        }

        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (Settings::get_instance().spectator_mode
            && platform->key(engine::platform::KEY_F).state() == engine::platform::Key::State::JustPressed) {
            Settings::get_instance().spectator_mode = false;
            auto gui_controller                     = engine::core::Controller::get<GuiController>();
            gui_controller->set_enable(true);
            Settings::get_instance().health = Settings::get_instance().max_health;
            set_camera_to_starting_position();
        }
    }

    void MainController::update_skull_facing() {
        float skull_speed = Settings::get_instance().skull_speed;

        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        float time    = platform->frame_time().current;

        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        bool skull_facing;

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(time * skull_speed),
                                         glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 skull_front(0.0f, 0.0f, 1.0f);
        skull_front = glm::vec3(rotation * glm::vec4(skull_front, 0.0f));

        float angle = glm::degrees(glm::acos(glm::dot(skull_front, -graphics->camera()->Front)));
        if (angle <= 60.0f) {
            skull_facing = true;
        } else {
            skull_facing = false;
        }
        Settings::get_instance().skull_facing_player = skull_facing;
    }

} // app
