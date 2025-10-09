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
            const float SENS = 2.1f;
            camera->rotate_camera(position.dx * SENS, position.dy);
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


     void MainController::draw_army_truck() {
         auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
         auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

         engine::resources::Model* truck = resources->model("army_truck");
         engine::resources::Shader* shader = resources->shader("basic");

         shader->use();
         shader->set_bool("isTruck", m_headlightsOn);
         shader->set_float("headGlowRadius",   0.07f);
         shader->set_float("headGlowStrength", m_headlightsOn ? 1.4f : 0.0f);

         shader->set_mat4("projection", graphics->projection_matrix());
         shader->set_mat4("view", graphics->camera()->view_matrix());
         glm::vec3 truckPos = glm::vec3(12.0f, -0.5f, -2.0f);
         glm::vec3 targetHouse = glm::vec3(-8.0f, -0.5f, -25.0f);
         glm::vec3 dir = glm::normalize(glm::vec3(targetHouse.x - truckPos.x, 0.0f, targetHouse.z - truckPos.z));
         float yaw = atan2(dir.x, dir.z);

         glm::mat4 model = glm::mat4(1.0f);
         model = glm::translate(model, truckPos);
         model = glm::rotate(model, yaw, glm::vec3(0.f,1.f,0.f));
         model = glm::scale(model, glm::vec3(0.009f));
         shader->set_mat4("model", model);
         truck->draw(shader);

     }


    void MainController::draw_house1() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model* house = resources->model("house2");
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();
        shader->set_vec3("lightDir", glm::vec3(-0.2f, -1.0f, -0.3f));
        shader->set_vec3("lightColor", glm::vec3(1.0f));
        shader->set_float("ambient", 0.25f);
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
        shader->set_vec3("lightDir", glm::vec3(-0.2f, -1.0f, -0.3f));
        shader->set_vec3("lightColor", glm::vec3(1.0f));
        shader->set_float("ambient", 0.25f);
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-8.0f, -0.5f, -25.0f));
        model = glm::rotate(model, 10.3f, glm::vec3(0.f,1.f,0.f));
        model = glm::scale(model, glm::vec3(0.8f));
        shader->set_mat4("model", model );
        house->draw(shader);
    }

    void MainController::draw_floor() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model* floor = resources->model("floor");
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(15.0f, -0.5f, -5.0f));
        model = glm::rotate(model, -0.1f, glm::vec3(0.f,1.f,0.f));
        model = glm::scale(model, glm::vec3(2.f));
        shader->set_mat4("model", model );
        floor->draw(shader);
    }

    void MainController::draw_chair() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model* chair = resources->model("chair");
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();
        shader->set_vec3("lightDir", glm::vec3(-0.2f, -1.0f, -0.3f));
        shader->set_vec3("lightColor", glm::vec3(1.0f));
        shader->set_float("ambient", 0.25f);
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-7.0f, -0.1f, -20.0f));
        model = glm::rotate(model, 1.2f, glm::vec3(0.f,1.f,0.f));
        model = glm::scale(model, glm::vec3(0.8f));
        shader->set_mat4("model", model );
        chair->draw(shader);
    }

    void MainController::draw_rocking_chair() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model* chair = resources->model("chair");
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();
        //shader->set_vec3("lightDir", glm::vec3(-0.2f, -1.0f, -0.3f));
        //shader->set_vec3("lightColor", glm::vec3(1.0f));
        //shader->set_float("ambient", 0.25f);
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());

        float angle = glm::radians(12.0f) * sinf(m_chairPhase);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-7.0f, -0.1f, -20.0f));
        model = glm::rotate(model, 1.2f, glm::vec3(0.f,1.f,0.f));
        model = glm::rotate(model, angle, glm::vec3(1.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(0.8f));
        shader->set_mat4("model", model );
        chair->draw(shader);
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
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

        bool tDown = platform->key(engine::platform::KeyId::KEY_T).is_down();
        if (tDown && !m_prevT) {
            m_headlightsOn = !m_headlightsOn;
            //spdlog::info("Headlights: {}", m_headlightsOn ? "ON" : "OFF");
        }
        m_prevT = tDown;
        m_chairPhase += 1.8f * platform->dt();

    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox = resources->skybox("another_skybox");
        auto shader = resources->shader("skybox");
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->draw_skybox(shader, skybox);
    }

   void MainController::draw() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        auto* shader = resources->shader("basic");
        shader->use();

        glm::vec3 truckPos(12.0f, -0.5f, -2.0f);
        glm::vec3 house2  (-8.0f, -0.5f, -25.0f);

        glm::vec3 dir = glm::normalize(glm::vec3(house2.x - truckPos.x, 0.0f, house2.z - truckPos.z));
        float yaw = atan2(dir.x, dir.z);

        glm::mat4 T(1.0f);
        T = glm::translate(T, truckPos);
        T = glm::rotate(T, yaw, glm::vec3(0,1,0));
        T = glm::scale(T, glm::vec3(0.009f));

        glm::mat3 B = glm::mat3(T);
        glm::vec3 right = glm::normalize(B * glm::vec3(1,0,0));
        glm::vec3 up    = glm::normalize(B * glm::vec3(0,1,0));
        glm::vec3 fwd   = glm::normalize(B * glm::vec3(0,0,1)); // izgubio 20min na ovo

        glm::vec3 origin = glm::vec3(T * glm::vec4(0,0,0,1));

        // podesavanje pozicije glow-a za farove
        glm::vec3 headBase = origin + up*1.3f + fwd*2.95f;
        glm::vec3 spotPos1 = headBase + right*0.45f;
        glm::vec3 spotPos2 = headBase - right*0.45f;

        glm::vec3 aim = glm::normalize(fwd + glm::vec3(0.0f, -0.08f, 0.0f));

        shader->set_vec3 ("viewPos", graphics->camera()->Position);
        shader->set_float("materialShininess", 32.0f);
        shader->set_float("specularStrength", 0.6f);

        shader->set_vec3("spotPos1",  spotPos1);
        shader->set_vec3("spotDir1",  aim);
        shader->set_vec3("spotPos2",  spotPos2);
        shader->set_vec3("spotDir2",  aim);

        shader->set_float("spotInner", glm::cos(glm::radians(11.0f)));
        shader->set_float("spotOuter", glm::cos(glm::radians(15.0f)));
        shader->set_float("spotConst", 0.4f);

        if (m_headlightsOn) {
            shader->set_vec3("spotColor",  glm::vec3(2.0f, 1.45f, 0.50f));
            shader->set_float("spotLin",   0.08f);
            shader->set_float("spotQuad",  0.01f);
        }else {
            shader->set_vec3("spotColor",  glm::vec3(0.0f));
            shader->set_float("spotLin",   1.0f);
            shader->set_float("spotQuad",  1.0f);
        }

        shader->set_float("ambient",   0.003f);
        shader->set_vec3("lightDir",   glm::vec3(-0.25f, -1.0f, -0.35f));
        shader->set_vec3("lightColor", glm::vec3(0.32f, 0.36f, 0.55f));

        shader->set_bool("isTruck", false);

        draw_floor();
        draw_house1();
        draw_house2();
        draw_army_truck();
        draw_rocking_chair();

        draw_skybox();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }
}// app