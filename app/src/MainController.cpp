//
// Created by filip on 1/22/25.
//

#include "Utils.hpp"

#include "GUIController.hpp"
#include "../../engine/libs/glfw/include/GLFW/glfw3.h"


void set_shader(engine::resources::Shader* shader, engine::graphics::GraphicsController*, const char*);


#define SUN_POSITION VECTOR3_A(0.0f, 20.0f, -15.0f)

namespace app{

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

    void MainController::draw_ground() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *ground = resources->model("ground");
        engine::resources::Shader *shader = resources->shader("basic");
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);
        ground->draw(shader);
    }
    /* TODO: FIX THIS ROUGH LOADING MODULES */
    void MainController::draw_house() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *kuca = resources->model("kuca2");
        engine::resources::Shader *shader = resources->shader("kuca2");
        shader->use();
        set_shader(shader, graphics, "HOUSE");
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -4.6f, -2.0f));
        model = glm::scale(model, glm::vec3(1.6f));
        shader->set_mat4("model", model);
        kuca->draw(shader);
    }

    void MainController::draw_tree1() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *tree = resources->model("tree1");
        engine::resources::Shader *shader = resources->shader("basic");
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -4.6f, 10.0f));
        model = glm::scale(model, glm::vec3(1.6f));
        shader->set_mat4("model", model);
        tree->draw(shader);
    }

    void MainController::draw_tree2() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *tree = resources->model("tree2");
        engine::resources::Shader *shader = resources->shader("basic");
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -4.6f, -10.0f));
        model = glm::scale(model, glm::vec3(1.6f));
        shader->set_mat4("model", model);
        tree->draw(shader);
    }

    void MainController::draw_sun() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *sun = resources->model("Sun");
        engine::resources::Shader *shader = resources->shader("basic");
        shader->use();
        set_shader(shader, graphics, "SUN");
        sun->draw(shader);
    }

    void MainController::draw_ufo() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *ufo = resources->model("ufo");
        engine::resources::Shader *shader = resources->shader("basic");
        shader->use();
        set_shader(shader,graphics, "UFO");
        ufo->draw(shader);
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
    void MainController::draw_skyboxes() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox    = resources->skybox("mountain_skybox");
        auto shader    = resources->shader("skybox");
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->draw_skybox(shader, skybox);
    }


    void MainController::draw() {

        draw_ground();
        draw_tree1();
        draw_tree2();
        draw_sun();
        draw_house();
        draw_ufo();

        draw_skyboxes();


    }
    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

} // namespace app


void set_shader(engine::resources::Shader *shader, engine::graphics::GraphicsController* graphics, const char* IDENTIFIER) {
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    if (IDENTIFIER == "HOUSE") {
        shader->set_vec3("dirLight.direction", SUN_POSITION);
        shader->set_float("material.shininess", 32);
        shader->set_vec3("dirLight.ambient", VECTOR3_A(0.2f, 0.2f, 0.2f));
        shader->set_vec3("dirLight.diffuse", VECTOR3_A(1.0f, 0.5f, 0.5f));
        shader->set_vec3("dirLight.specular", VECTOR3_A(1.0f, 1.0f, 1.0f));
    }else if (IDENTIFIER == "SUN") {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, SUN_POSITION);
        model = glm::scale(model, VECTOR3_A(0.1f,0.1f,0.1f));
        shader->set_mat4("model", model);
    }else if (IDENTIFIER == "UFO") {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, VECTOR3_A(0.0f, 0.0f, -3.0f));
        model = glm::scale(model, VECTOR3_A(0.2f,0.2f,0.2f));
        shader->set_mat4("model", model);
    }
};