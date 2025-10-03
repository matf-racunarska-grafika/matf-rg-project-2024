//
// Created by filip on 1/22/25.
//

#include "Utils.hpp"
#include "MainController.hpp"

#include <iostream>

#include "GUIController.hpp"
#include "../../engine/libs/glfw/include/GLFW/glfw3.h"

void set_shader(engine::resources::Shader* shader, engine::graphics::GraphicsController*, const char*);


#define MAXCNUM 5
#define POINT_LIGHT_NUM 2
#define POSITIONLAMP1 VECTOR3(2.0f, -5.0f, 1.0f)
#define POSITIONLAMP2 VECTOR3(-0.5f, -5.0f, 1.0f)
#define TREE1POSITION VECTOR3(0.0f, -4.6f, 10.0f)

VECTOR3 SUN_POSITION = VECTOR3(0.0f,20.f,-15.0f);
VECTOR3 SUN_DIFFUSE = VECTOR3(1.0f, 0.5f, 0.5f);
VECTOR3 SUN_AMBIENT = VECTOR3(0.2f,0.2f, 0.2f);

VECTOR3 UFO_POSITON = VECTOR3(0.0f, 0.0f, -3.0f);
VECTOR3 HOUSE_POSITION = VECTOR3(0.0f, -5.05f, -2.0f);

VECTOR3 POSITIONLAMPS[2] = {POSITIONLAMP1, POSITIONLAMP2};

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
        static int CNUM = 0;
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        if (platform->key(engine::platform::KeyId::KEY_G).is_down()) {
            SUN_POSITION = VECTOR3(cos(glfwGetTime())*10.0f, 10.0f, 3.0+(float)sin(glfwGetTime())*7.0f);
        }
        if (platform->key(engine::platform::KeyId::KEY_C).is_down()) {
            if (CNUM >= MAXCNUM) {
                return true;
            }
            SUN_AMBIENT += VECTOR3(0.01f, 0.01f, 0.01f);
            SUN_DIFFUSE += VECTOR3(0.0f,0.2f,0.1f);
            ++CNUM;
        }
        if (platform->key(engine::platform::KeyId::KEY_T).is_down()) {
            UFO_POSITON -= VECTOR3(0.0f, -0.15f, 0.0f);
            HOUSE_POSITION -= VECTOR3(0.0f, -0.1f, 0.0f);
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
        model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);
        ground->draw(shader);
    }

    void MainController::draw_house() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *kuca = resources->model("kuca2");
        engine::resources::Shader *shader = resources->shader("kuca2");
        shader->use();
        set_shader(shader, graphics, "HOUSE");

        kuca->draw(shader);
    }

    void MainController::draw_tree1() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *tree = resources->model("tree1");
        engine::resources::Shader *shader = resources->shader("tree1");
        shader->use();
        set_shader(shader, graphics, "TREE");

        const long instanced = 5;

        if (tree1_models.empty()) {

            float y = TREE1POSITION.y;
            for (int i = 0; i < instanced; i++) {
                float x = TREE1POSITION.x+(float)(5.0f+sin(glfwGetTime())*i);
                float z = TREE1POSITION.z+(float)(5.0f+cos(glfwGetTime())*i);
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(x, y, z));
                printf("uso\n");

                model = glm::scale(model, glm::vec3(2.6f));
                tree1_models.push_back(model);
            }
        }

        if (tree->initialization_instances(tree1_models) != 0) {
            throw std::runtime_error("Failed to initialize instances");
        }
        if (tree->draw_instanced(shader, instanced) != 0) {
            throw std::runtime_error("Failed to draw instances");
        }
    }

    void MainController::draw_tree2() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *tree = resources->model("tree2");
        engine::resources::Shader *shader = resources->shader("tree2");
        shader->use();
        set_shader(shader, graphics, "TREE");
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

    void MainController::draw_lamp() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *lamp = resources->model("lamp");
        engine::resources::Shader *shader = resources->shader("lamp");
        shader->use();
        set_shader(shader, graphics, "LAMP");
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, POSITIONLAMP1);
        model = glm::scale(model, VECTOR3(0.1f,0.1f,0.1f));
        shader->set_mat4("model", model);

        lamp->draw(shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, POSITIONLAMP2);
        model = glm::scale(model, VECTOR3(0.1f,0.1f,0.1f));
        shader->set_mat4("model", model);
        lamp->draw(shader);
    }

    void MainController::draw_ufo() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *ufo = resources->model("ufo");
        engine::resources::Shader *shader = resources->shader("ufo");
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
        draw_lamp();
        //draw_skyboxes();
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
        shader->set_vec3("dirLight.ambient", SUN_AMBIENT);
        shader->set_vec3("dirLight.diffuse", SUN_DIFFUSE);
        shader->set_vec3("dirLight.specular", VECTOR3(1.0f, 1.0f, 1.0f));
        for (int i = 0; i < POINT_LIGHT_NUM; i++) {
            std::string s = "pointLights[" + std::to_string(i) + "].";

            shader->set_vec3(s + "position", POSITIONLAMPS[i]);
            shader->set_float(s + "constant", 1.0f);
            shader->set_float(s + "linear", 0.09f);
            shader->set_float(s + "quadratic", 0.032f);
            shader->set_vec3(s + "ambient", VECTOR3(0.1f,0.1f,0.1f));
            shader->set_vec3(s + "diffuse", VECTOR3(0.3f, 0.3f, 0.3f));
            shader->set_vec3(s + "specular", VECTOR3(1.0f, 1.0f, 1.0f));
        }
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, HOUSE_POSITION);
        model = glm::scale(model, glm::vec3(2.6f));
        shader->set_mat4("model", model);
    }else if (IDENTIFIER == "SUN") {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, SUN_POSITION);
        model = glm::scale(model, VECTOR3(0.1f,0.1f,0.1f));
        shader->set_mat4("model", model);
    }else if (IDENTIFIER == "UFO") {
        shader->set_vec3("dirLight.direction", SUN_POSITION);
        shader->set_float("material.shininess", 32);
        shader->set_vec3("dirLight.ambient", SUN_AMBIENT);
        shader->set_vec3("dirLight.diffuse", SUN_DIFFUSE);
        shader->set_vec3("dirLight.specular", VECTOR3(1.0f, 1.0f, 1.0f));

        for (int i = 0; i < POINT_LIGHT_NUM; i++) {
            std::string s = "pointLights[" + std::to_string(i) + "].";

            shader->set_vec3(s + "position", POSITIONLAMPS[i]);
            shader->set_float(s + "constant", 1.0f);
            shader->set_float(s + "linear", 0.09f);
            shader->set_float(s + "quadratic", 0.032f);
            shader->set_vec3(s + "ambient", VECTOR3(0.1f,0.1f,0.1f));
            shader->set_vec3(s + "diffuse", VECTOR3(0.3f, 0.3f, 0.3f));
            shader->set_vec3(s + "specular", VECTOR3(1.0f, 1.0f, 1.0f));
        }
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, UFO_POSITON);
        model = glm::scale(model, VECTOR3(0.2f,0.2f,0.2f));
        shader->set_mat4("model", model);
    }else if (IDENTIFIER == "LAMP") {
        shader->set_vec3("dirLight.direction", SUN_POSITION);
        shader->set_float("material.shininess", 32);
        shader->set_vec3("dirLight.ambient", SUN_AMBIENT);
        shader->set_vec3("dirLight.diffuse", SUN_DIFFUSE);
        shader->set_vec3("dirLight.specular", VECTOR3(1.0f, 1.0f, 1.0f));

        for (int i = 0; i < POINT_LIGHT_NUM; i++) {
            std::string s = "pointLights[" + std::to_string(i) + "].";

            shader->set_vec3(s + "position", POSITIONLAMPS[i]);
            shader->set_float(s + "constant", 1.0f);
            shader->set_float(s + "linear", 0.09f);
            shader->set_float(s + "quadratic", 0.032f);
            shader->set_vec3(s + "ambient", VECTOR3(0.1f,0.1f,0.1f));
            shader->set_vec3(s + "diffuse", VECTOR3(0.3f, 0.3f, 0.3f));
            shader->set_vec3(s + "specular", VECTOR3(1.0f, 1.0f, 1.0f));
        }
    }else if (IDENTIFIER == "TREE") {
        shader->set_vec3("dirLight.direction", SUN_POSITION);
        shader->set_float("material.shininess", 32);

        shader->set_vec3("dirLight.ambient", SUN_AMBIENT);
        shader->set_vec3("dirLight.diffuse", SUN_DIFFUSE);
        shader->set_vec3("dirLight.specular", VECTOR3(1.0f, 1.0f, 1.0f));

        for (int i = 0; i < POINT_LIGHT_NUM; i++) {
            std::string s = "pointLights[" + std::to_string(i) + "].";

            shader->set_vec3(s + "position", POSITIONLAMPS[i]);
            shader->set_float(s + "constant", 1.0f);
            shader->set_float(s + "linear", 0.09f);
            shader->set_float(s + "quadratic", 0.032f);
            shader->set_vec3(s + "ambient", VECTOR3(0.1f,0.1f,0.1f));
            shader->set_vec3(s + "diffuse", VECTOR3(0.3f, 0.3f, 0.3f));
            shader->set_vec3(s + "specular", VECTOR3(1.0f, 1.0f, 1.0f));
        }
    }
};


/*
 *TODO:
 *dodaj da se pokupi kuca na slovo c npr
 *i nlo se dize gore i lampe
 *drvo ne mora
 *INSTANCING dodaj samo one komete iz learnopengl sajta
 */