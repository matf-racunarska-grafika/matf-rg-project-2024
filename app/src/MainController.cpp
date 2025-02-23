#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/graphics/PostProcessingHandler.hpp>
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
        PostProcessingHandler::initialise();
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
        float time = platform->frame_time().current;
        // Model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* skullModel = resources->model("skull");

        // Shader
        engine::resources::Shader* shader = resources->shader("NearShader");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.5f, -46.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

        // Speed of the skull rotation
        float skullSpeed = Settings::getInstance().skullSpeed;
        model = glm::rotate(model, glm::radians(time * skullSpeed), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader->set_mat4("model", model);

        shader->set_vec3("viewPosition", graphics->camera()->Position);
        shader->set_float("material.shininess", 32.0f);

        shader->set_vec3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -0.7f));
        shader->set_vec3("dirLight.ambient", glm::vec3(0.05f, 0.01f, 0.01f));
        shader->set_vec3("dirLight.diffuse", glm::vec3(0.05f, 0.01f, 0.01f));
        shader->set_vec3("dirLight.specular", glm::vec3(0.05f, 0.01f, 0.01f));

        // getting the possitions of the eyes
        glm::vec3 eyePos1;
        glm::vec3 eyePos2;
        glm::mat4 modelEye1 = model;
        glm::mat4 modelEye2 = model;

        modelEye1 = glm::translate(modelEye1, glm::vec3(-4.4f, -9.8f, 14.0f));
        modelEye1 = glm::scale(modelEye1, glm::vec3(0.008f));

        modelEye2 = glm::translate(modelEye2, glm::vec3(4.4f, -9.8f, 14.0f));
        modelEye2 = glm::scale(modelEye2, glm::vec3(0.008f));

        glm::vec4 eye1Pos = modelEye1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec4 eye2Pos = modelEye2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        glm::vec3 lightColor = Settings::getInstance().lightColor;
        shader->set_vec3("pointLights[0].position", glm::vec3(eye1Pos));
        shader->set_vec3("pointLights[0].ambient", glm::vec3(0.0f));
        shader->set_vec3("pointLights[0].diffuse", lightColor);
        shader->set_vec3("pointLights[0].specular", lightColor);
        shader->set_float("pointLights[0].constant", 10.0f);
        shader->set_float("pointLights[0].linear", 0.09f);
        shader->set_float("pointLights[0].quadratic", 0.032f);

        shader->set_vec3("pointLights[1].position", glm::vec3(eye2Pos));
        shader->set_vec3("pointLights[1].ambient", glm::vec3(0.0f));
        shader->set_vec3("pointLights[1].diffuse", lightColor);
        shader->set_vec3("pointLights[1].specular", lightColor);
        shader->set_float("pointLights[1].constant", 10.0f);
        shader->set_float("pointLights[1].linear", 0.09f);
        shader->set_float("pointLights[1].quadratic", 0.032f);

        skullModel->draw(shader);

        draw_eyes(modelEye1, modelEye2);
    }

    void MainController::draw_bridge() {
        // Model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* bridgeModel = resources->model("bridge");

        // Shader
        engine::resources::Shader* shader = resources->shader("NearShader");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("model", glm::mat4(1.0f));
        glm::mat4 model = glm::mat4(1.0f);
        for (int i = 0; i <= 7; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -2.0f, -i*3.39f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f));
            shader->set_mat4("model", model);

            bridgeModel->draw(shader);
        }
    }

    void MainController::draw_eyes(glm::mat4 eye1Model, glm::mat4 eye2Model) {
        // Model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* eyeModel = resources->model("eyes");

        // Shader
        engine::resources::Shader* shader = resources->shader("eyeShader");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_vec3("lightColor", Settings::getInstance().lightColor);

        shader->set_mat4("model", eye1Model);
        eyeModel->draw(shader);
        shader->set_mat4("model", eye2Model);
        eyeModel->draw(shader);
    }


    void MainController::draw_arena() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        float time = platform->frame_time().current;

        // Model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* arenaModel = resources->model("arena");


        // Shader
        engine::resources::Shader* shader = resources->shader("NearShader");
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -20.0f, -10.0f));
        model = glm::rotate(model, glm::radians(time), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.008f));
        shader->set_mat4("model", model);

        shader->set_vec3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -0.7f));
        shader->set_vec3("dirLight.ambient", glm::vec3(0.05f, 0.01f, 0.01f));
        shader->set_vec3("dirLight.diffuse", glm::vec3(0.05f, 0.01f, 0.01f));
        shader->set_vec3("dirLight.specular", glm::vec3(0.05f, 0.01f, 0.01f));

        arenaModel->draw(shader);
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox = resources->skybox("skybox");
        auto shader = resources->shader("skybox");
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->draw_skybox(shader, skybox);
    }

    void MainController::draw() {
        draw_bridge();
        draw_skull();
        draw_arena();
        draw_skybox();

        PostProcessingHandler::unbind_framebuffer();
        PostProcessingHandler::draw();
    }

    void MainController::begin_draw() {
        PostProcessingHandler::bind_hdr_framebuffer();
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::end_draw() {
        PostProcessingHandler::unbind_framebuffer();
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

    void MainController::update_camera() {
        auto gui_controller = engine::core::Controller::get<GuiController>();
        if (gui_controller->is_enabled()) {
            return;
        }
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        float dt = platform->dt();
        bool skullFacing = Settings::getInstance().skullFacingPlayer;

        if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            if (!skullFacing) {
                camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
            } else {
                // teleport player to start if he has been spotted
                camera->Position = glm::vec3(0.0f, 0.0f, 0.0f);
            }
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
            camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_SPACE).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::UP, dt);
        }
    }

    void MainController::update() {
        update_camera();
        update_skull_facing();
    }

    glm::vec3 rotate_vector(const glm::vec3& vec, float angleDegrees) {
        float angleRadians = glm::radians(angleDegrees);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        return glm::vec3(rotation * glm::vec4(vec, 0.0f));
    }

    void MainController::update_skull_facing() {
        float skullSpeed = Settings::getInstance().skullSpeed;

        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        float time = platform->frame_time().current;

        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        bool skullFacing;

        glm::vec3 skullFront = rotate_vector(glm::vec3(0.0f, 0.0f, 1.0f), skullSpeed * time);
        float angle = glm::degrees(glm::acos(glm::dot(skullFront, -graphics->camera()->Front)));

        if (angle <= 60.0f) {
            skullFacing = true;
        } else {
            skullFacing = false;
        }
        Settings::getInstance().skullFacingPlayer = skullFacing;
    }

} // app