//
// Created by filipn on 9/1/25.
//

#include <GLFW/glfw3.h>

#include <GuiController.hpp>
#include <MainController.hpp>
#include <engine/graphics/Framebuffer.hpp>

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

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
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());

    platform->set_enable_cursor(false);

    auto graphics = engine::graphics::GraphicsController::get<engine::graphics::GraphicsController>();
    graphics->init_msaa_framebuffer(platform->window()->width(), platform->window()->height());

    engine::graphics::OpenGL::enable_depth_testing();

    graphics->add_point_shadow_caster();// moon
    graphics->add_point_shadow_caster();// sun
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_ESCAPE).is_down()) { return false; }
    return true;
}

void MainController::draw_moon() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *moon = resources->model("moon");

    engine::resources::Shader *shader = resources->shader("sun_moon");
    shader->use();

    float current_time = glfwGetTime();
    auto gui_controller = engine::core::Controller::get<GuiController>();
    float delta = 0.05f * gui_controller->get_revolution_speed();

    PointLight moonlight;

    moonlight.position = glm::vec3(0.0f, 6.0f * sin(current_time * delta) - 2, -6.0f * cos(current_time * delta));
    moonlight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    moonlight.diffuse = glm::vec3(0.2f, 0.2f, 0.6f);
    moonlight.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    moonlight.constant = 1.0f;
    moonlight.linear = 0.2f;
    moonlight.quadratic = 0.035f;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, moonlight.position);
    model = glm::scale(model, glm::vec3(0.3f));

    shader->set_mat4("model", model);
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    moon->draw(shader);

    engine::resources::Shader *shaderShadow = resources->shader("point_light_shadows");
    graphics->set_shadow_caster_position(moonlight.position, 0);
    graphics->render_point_light_shadows(shaderShadow, 0, [&](engine::resources::Shader *shadowPassShader) {
        draw_island(shadowPassShader);
        draw_bench(shadowPassShader);
        draw_bush(shadowPassShader);
        draw_lamp(shadowPassShader, true);
        draw_path(shadowPassShader);
        draw_tree(shadowPassShader);
    });

    engine::resources::Shader *shaderMain = resources->shader("basic");
    graphics->bind_point_light_shadows_to_shader(shaderMain, 0, "moon");
    set_point_light(shaderMain, moonlight, "moon");

    graphics->bind_msaa_framebuffer();
}

void MainController::draw_sun() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *sun = resources->model("sun");

    engine::resources::Shader *shader = resources->shader("sun_moon");
    shader->use();

    float current_time = glfwGetTime();
    auto gui_controller = engine::core::Controller::get<GuiController>();
    float delta = 0.05f * gui_controller->get_revolution_speed();

    PointLight sunlight;

    sunlight.position = glm::vec3(0.0f, -6.0f * sin(current_time * delta) - 2, 6.0f * cos(current_time * delta));
    sunlight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    sunlight.diffuse = glm::vec3(1.0f, 0.6f, 0.9f);
    sunlight.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    sunlight.constant = 1.0f;
    sunlight.linear = 0.01f;
    sunlight.quadratic = 0.002f;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, sunlight.position);
    model = glm::scale(model, glm::vec3(0.3f));

    shader->set_mat4("model", model);
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    sun->draw(shader);

    engine::resources::Shader *shaderShadow = resources->shader("point_light_shadows");
    graphics->set_shadow_caster_position(sunlight.position, 1);
    graphics->render_point_light_shadows(shaderShadow, 1, [&](engine::resources::Shader *shadowPassShader) {
        draw_island(shadowPassShader);
        draw_bench(shadowPassShader);
        draw_bush(shadowPassShader);
        draw_lamp(shadowPassShader, true);
        draw_path(shadowPassShader);
        draw_tree(shadowPassShader);
    });

    engine::resources::Shader *shaderMain = resources->shader("basic");
    graphics->bind_point_light_shadows_to_shader(shaderMain, 1, "sun");
    set_point_light(shaderMain, sunlight, "sun");

    graphics->bind_msaa_framebuffer();
}

void MainController::draw_island(engine::resources::Shader *shader) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *island = resources->model("island");
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-4.0f, 14.37f, -3.5f));
    model = glm::scale(model, glm::vec3(0.1f));

    shader->set_mat4("model", model);
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    shader->set_vec3("viewPosition", graphics->camera()->Position);

    island->draw(shader);
}

void MainController::draw_tree(engine::resources::Shader *shader) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *tree = resources->model("tree");
    shader->use();

    glm::vec3 positions[] = {
            glm::vec3(0.0f, 0.0f, -0.5f),
            glm::vec3(-1.5f, 0.0f, -1.0f),
            glm::vec3(-1.0f, 0.0f, 1.0f),
            glm::vec3(0.5f, 0.0f, 0.8f),
            glm::vec3(1.5f, 0.0f, 0.2f),
            glm::vec3(1.0f, 0.0f, -1.5f),
            glm::vec3(-0.6f, 0.0f, -2.1f),
            glm::vec3(-2.5f, -1.515f, -0.6f),
            glm::vec3(-2.4f, -1.515f, -1.8f),
            glm::vec3(0.0f, -3.25f, -2.5f)
    };

    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    for (int i = 0; i < 10; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[i]);
        model = glm::rotate(model, glm::radians(i * 45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);

        tree->draw(shader);
    }
}

void MainController::draw_bench(engine::resources::Shader *shader) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *bench = resources->model("bench");

    shader->use();// (-0.5f, 0.0f, -0.28f) // (0.5f, 0.0f, -0.28f)

    glm::vec3 positions[] = {
            glm::vec3(-0.5f, 0.0f, -0.28f),
            glm::vec3(0.5f, 0.0f, -0.28f)
    };
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    for (int i = 0; i < 2; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[i]);
        model = glm::rotate(model, glm::radians(180.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.05f));
        shader->set_mat4("model", model);

        bench->draw(shader);
    }
}

void MainController::draw_lamp(engine::resources::Shader *shader, bool shadowPass) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *lamp = resources->model("lamp");
    shader->use();

    glm::vec3 positions[] = {
            glm::vec3(-1.0f, 0.0f, -0.28f),
            glm::vec3(0.0f, 0.0f, 0.67f),
            glm::vec3(1.0f, 0.0f, -0.28f),
            glm::vec3(0.0f, 0.0f, -1.23f),//0.67, 0.1
    };

    glm::vec3 lightPositions[] = {
            glm::vec3(-0.9f, 0.67f, -0.28f),
            glm::vec3(0.0f, 0.67f, 0.57f),
            glm::vec3(0.9f, 0.67f, -0.28f),
            glm::vec3(0.0f, 0.67f, -1.13f),//0.67, 0.1
    };

    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    for (int i = 0; i < 4; i++) {
        if (!shadowPass) { set_spot_light(shader, i, lightPositions[i]); }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[i]);
        model = glm::rotate(model, glm::radians(i * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        shader->set_mat4("model", model);

        lamp->draw(shader);
    }
}

void MainController::draw_bush(engine::resources::Shader *shader) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *bush = resources->model("bush");

    shader->use();

    glm::vec3 positions[] = {
            glm::vec3(-0.2f, 0.13f, 0.1f),
            glm::vec3(0.2f, 0.13f, -2.0f),
            glm::vec3(-0.5f, 0.13f, 1.2f),
            glm::vec3(-0.7f, 0.13f, -1.5f),
            glm::vec3(0.9f, 0.13f, 1.0f),
            glm::vec3(-1.3f, 0.13f, 0.5f),
            glm::vec3(1.1f, 0.13f, -0.5f),
            glm::vec3(-1.6f, 0.13f, -0.3f),
            glm::vec3(1.7f, 0.13f, -1.1f),
            glm::vec3(-3.0f, -1.38f, -1.0f),
            glm::vec3(-2.3f, -1.38f, 0.1f),
            glm::vec3(-1.7f, -1.38f, -1.5f),
            glm::vec3(-0.7f, -3.11f, -1.9f),
            glm::vec3(0.5f, -3.11f, -2.1f),
            glm::vec3(-0.9f, -3.11f, -2.6f),
    };

    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    for (int i = 0; i < 15; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[i]);
        model = glm::rotate(model, glm::radians(i * 50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.0025f));
        shader->set_mat4("model", model);

        bush->draw(shader);
    }
}

void MainController::draw_path(engine::resources::Shader *shader) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *path = resources->model("path");

    shader->use();// (-0.8f, 0.0f, -0.5f) // (0.8f, 0.0f, -0.5f) // (-0.8f, 0.0f, -0.075f) // (0.8f, 0.0f, -0.075f)

    glm::vec3 positions[] = {
            glm::vec3(-0.8f, 0.0f, -0.5f),
            glm::vec3(0.8f, 0.0f, -0.5f),
            glm::vec3(-0.8f, 0.0f, -0.075f),
            glm::vec3(0.8f, 0.0f, -0.075f),
    };

    glm::vec3 axes[] = {
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
    };

    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("projection", graphics->projection_matrix());

    for (int i = 0; i < 4; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[i]);
        if (i > 0) { model = glm::rotate(model, glm::radians(180.0f), axes[i - 1]); }
        model = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);

        path->draw(shader);
    }
}

void MainController::set_point_light(engine::resources::Shader *shader, const PointLight &pointLight, const std::string &lightName) {
    shader->use();
    shader->set_vec3(lightName + ".position", pointLight.position);

    shader->set_vec3(lightName + ".ambient", pointLight.ambient);
    shader->set_vec3(lightName + ".diffuse", pointLight.diffuse);
    shader->set_vec3(lightName + ".specular", pointLight.specular);

    shader->set_float(lightName + ".constant", pointLight.constant);
    shader->set_float(lightName + ".linear", pointLight.linear);
    shader->set_float(lightName + ".quadratic", pointLight.quadratic);
}

void MainController::set_spot_light(engine::resources::Shader *shader, int i, glm::vec3 lightPosition) {
    auto gui_controller = engine::core::Controller::get<GuiController>();
    glm::vec3 lampColor = gui_controller->get_lamp_color();

    shader->use();
    shader->set_vec3("lamps[" + std::to_string(i) + "].ambient", glm::vec3(0.0f));
    shader->set_vec3("lamps[" + std::to_string(i) + "].diffuse", 0.8f * lampColor);
    shader->set_vec3("lamps[" + std::to_string(i) + "].specular", 0.5f * lampColor);

    shader->set_vec3("lamps[" + std::to_string(i) + "].position", lightPosition);
    shader->set_vec3("lamps[" + std::to_string(i) + "].direction", glm::vec3(0.0f, -1.0f, 0.0f));

    shader->set_float("lamps[" + std::to_string(i) + "].cutOff", glm::cos(glm::radians(25.0f)));
    shader->set_float("lamps[" + std::to_string(i) + "].outerCutOff", glm::cos(glm::radians(45.0f)));

    shader->set_float("lamps[" + std::to_string(i) + "].constant", 1.0f);
    shader->set_float("lamps[" + std::to_string(i) + "].linear", 0.1f);
    shader->set_float("lamps[" + std::to_string(i) + "].quadratic", 0.03f);
}

void MainController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GuiController>();
    if (gui_controller->is_enabled()) { return; }

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KEY_W).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt); }
    if (platform->key(engine::platform::KEY_A).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }
    if (platform->key(engine::platform::KEY_S).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt); }
    if (platform->key(engine::platform::KEY_D).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt); }
    if (platform->key(engine::platform::KEY_SPACE).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::UP, dt); }
    if (platform->key(engine::platform::KEY_LEFT_SHIFT).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt); }


}

void MainController::update() { update_camera(); }

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->bind_msaa_framebuffer();
    engine::graphics::OpenGL::clear_buffers();
    engine::graphics::OpenGL::enable_depth_testing();
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("blue");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw_quad() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    graphics->resolve_msaa(platform->window()->width(), platform->window()->height());
    graphics->unbind_msaa_framebuffer();
    engine::graphics::OpenGL::clear_buffers();
    engine::graphics::OpenGL::disable_depth_testing();

    auto shader = resources->shader("aa_post");

    graphics->draw_quad(shader);
}

void MainController::draw() {
    draw_moon();
    draw_sun();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");

    draw_lamp(shader);
    draw_island(shader);
    draw_tree(shader);
    draw_bench(shader);
    draw_bush(shader);
    draw_path(shader);
    draw_skybox();
    draw_quad();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

}// app