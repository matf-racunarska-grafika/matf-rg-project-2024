//
// Created by ognje on 7/14/2025.
//

#include <MainController.hpp>
#include <GuiController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

namespace app {

    class MainPlatformObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto gui_controller = engine::core::Controller::get<GUIController>();
        if (!gui_controller->is_enabled()) {
            auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
            camera->rotate_camera(position.dx, position.dy);
        }
    }

    void MainController::initialize() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->register_platform_event_observer(std::make_unique<MainPlatformObserver>());
        engine::graphics::OpenGL::enable_depth_testing();
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
        return true;
    }

    static glm::vec3 circle_pos(float radius, float angleDeg, glm::vec3 center) {
            float a = glm::radians(angleDeg);
            return center + glm::vec3(radius * cos(a), 0.0f, radius * sin(a));
        }
    static float yaw_to_face(glm::vec3 from, glm::vec3 to) {
            glm::vec3 d = glm::normalize(glm::vec3(to.x - from.x, 0.0f, to.z - from.z));
            return atan2(d.x, d.z); // rotacija oko Y da gleda ka 'to'
        }
    static void set_normal_matrix(engine::resources::Shader* shader, const glm::mat4& model) {
            glm::mat3 N = glm::mat3(glm::transpose(glm::inverse(model)));
            shader->set_mat3("normalMatrix", N);
        }

    void MainController::draw_backpack() {
        //Model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model* backpack = resources->model("backpack");
        //Shader
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model );
        backpack->draw(shader);
    }

     void MainController::draw_army_truck() {
         auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
         auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

         engine::resources::Model* truck = resources->model("army_truck");
         engine::resources::Shader* shader = resources->shader("basic");

         shader->use();
         shader->set_mat4("projection", graphics->projection_matrix());
         shader->set_mat4("view", graphics->camera()->view_matrix());
         glm::mat4 model = glm::mat4(1.0f);
         model = glm::translate(model, glm::vec3(12.0f, -0.5f, -2.0f));
         model = glm::rotate(model, -0.5f, glm::vec3(0.f,1.f,0.f));
         model = glm::scale(model, glm::vec3(0.009f));
         shader->set_mat4("model", model );
         truck->draw(shader);

     }


    void MainController::draw_house1() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model* house = resources->model("house2");
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-8.0f, -0.5f, -2.0f));
        //model = glm::rotate(model, -0.5f, glm::vec3(0.f,1.f,0.f));
        model = glm::scale(model, glm::vec3(0.8f));
        shader->set_mat4("model", model );
        house->draw(shader);
    }

    void MainController::draw_house2() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model* house = resources->model("house2");
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-8.0f, -0.5f, -25.0f));
        model = glm::rotate(model, 10.3f, glm::vec3(0.f,1.f,0.f));
        model = glm::scale(model, glm::vec3(0.8f));
        shader->set_mat4("model", model );
        house->draw(shader);
    }


    void MainController::draw_lamp() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model* lamp = resources->model("lamp");
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(15.0f, -0.5f, -5.0f));
        model = glm::rotate(model, -0.1f, glm::vec3(0.f,1.f,0.f));
        model = glm::scale(model, glm::vec3(0.5f));
        shader->set_mat4("model", model );
        lamp->draw(shader);
    }


    void MainController::update_camera() {
        auto gui_controller = engine::core::Controller::get<GUIController>();
        if (gui_controller->is_enabled()) {
            return;
        }
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        float dt = platform->dt();

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

    }

    void MainController::update() {
        update_camera();
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox = resources->skybox("mountain_skybox");
        auto shader = resources->shader("skybox");
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->draw_skybox(shader, skybox);
    }

    void MainController::draw() {
        //draw_backpack();
        draw_house1();
        draw_house2();
        draw_army_truck();
        //draw_lamp();
        draw_skybox();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }
}// app