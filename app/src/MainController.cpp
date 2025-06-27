#include <memory>
#include <spdlog/spdlog.h>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <app/MainController.hpp>
#include <app/GUIController.hpp>

namespace engine::myapp {
void MainPlatformEventObserver::on_key(engine::platform::Key key) { spdlog::info("Keyboard event: key={}, state={}", key.name(), key.state_str()); }

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) { spdlog::info("MousePosition: {} {}", position.x, position.y); }

void MainController::initialize() {
    // User initialization
    engine::graphics::OpenGL::enable_depth_testing();

    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(
            std::move(observer));
}

bool MainController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) { return false; }
    return true;
}

void MainController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1)
                .state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    }
}

void MainController::update() { update_camera(); }

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

// Scena
void MainController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    auto BasicShader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    BasicShader->use();
    BasicShader->set_mat4("projection", graphics->projection_matrix());
    BasicShader->set_mat4("view", graphics->camera()->view_matrix());
    BasicShader->set_vec3("viewPos", graphics->camera()->Position);

    auto PointLightShader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("lighting");
    PointLightShader->use();
    PointLightShader->set_mat4("projection", graphics->projection_matrix());
    PointLightShader->set_mat4("view", graphics->camera()->view_matrix());
    PointLightShader->set_vec3("viewPos", graphics->camera()->Position);

    // Crtaj point light-ove
    set_point_lights(PointLightShader);

    // Crtaj modele
    auto backpack = engine::core::Controller::get<engine::resources::ResourcesController>()->model("backpack");
    draw_mesh(backpack, PointLightShader, glm::vec3(-5.0f, 0.0f, -7.0f), glm::vec3(0.0f), glm::vec3(0.5f));
    draw_mesh(backpack, PointLightShader, glm::vec3(5.0f, 0.0f, -7.0f), glm::vec3(0.0f), glm::vec3(0.5f));

    auto terrain = engine::core::Controller::get<engine::resources::ResourcesController>()->model("terrain");
    draw_mesh(terrain, PointLightShader, glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f), glm::vec3(3.0f));

    // Crtaj skybox
    draw_skybox();
}

void MainController::end_draw() { engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers(); }

// USER DEFINED
// ---------------------------------------------------------------------------------------------------------------------------

void MainController::draw_mesh(auto model, auto shader,
                               const glm::vec3 &position,
                               const glm::vec3 &rotation,// Eulerovi uglovi u radijanima: (rotX, rotY, rotZ)
                               const glm::vec3 &scale) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, position);
    modelMat = glm::rotate(modelMat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::rotate(modelMat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::rotate(modelMat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMat = glm::scale(modelMat, scale);
    shader->set_mat4("model", modelMat);

    model->draw(shader);
}

void MainController::draw_light_source_mesh(const glm::vec3 &lightPos, float scale) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto basicShader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("unlit_white");
    auto cubeModel = engine::core::Controller::get<engine::resources::ResourcesController>()->model("light_bulb");

    basicShader->use();
    basicShader->set_mat4("projection", graphics->projection_matrix());
    basicShader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, lightPos);
    modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(scale));
    basicShader->set_mat4("model", modelMat);

    cubeModel->draw(basicShader);
}

void MainController::set_point_lights(auto shader) {
    // Pozicije point lightova
    glm::vec3 lightPos0 = glm::vec3(-5.0, 1.0f, 2.0f);
    glm::vec3 lightPos1 = glm::vec3(0.0f, 1.0f, 2.0f);

    // Postavljanje uniform-e za prvi point light
    shader->set_vec3("pointLights[0].position", lightPos0);
    shader->set_float("pointLights[0].constant", 1.0f);
    shader->set_float("pointLights[0].linear", 0.09);
    shader->set_float("pointLights[0].quadratic", 0.032);
    shader->set_vec3("pointLights[0].ambient", glm::vec3(0.2));
    shader->set_vec3("pointLights[0].diffuse", glm::vec3(0.5));
    shader->set_vec3("pointLights[0].specular", glm::vec3(0.3));

    // Postavljanje uniform-e za drugi point light
    shader->set_vec3("pointLights[1].position", lightPos1);
    shader->set_float("pointLights[1].constant", 1.0f);
    shader->set_float("pointLights[1].linear", 0.09);
    shader->set_float("pointLights[1].quadratic", 0.032);
    shader->set_vec3("pointLights[1].ambient", glm::vec3(0.2));
    shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.5));
    shader->set_vec3("pointLights[1].specular", glm::vec3(0.3));

    // Mesh na poziciji izvora point light-ova
    draw_light_source_mesh(lightPos0, 3.0f);// prvi izvor svetla
    draw_light_source_mesh(lightPos1, 3.0f);// drugi izvor svetla
}

void MainController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

void MainController::update_camera() {
    auto gui = engine::core::Controller::get<GUIController>();
    if (gui->is_enabled()) { return; }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();

    // Ako je SHIFT pritisnut, povecaj faktor brzine
    float speedMultiplier = 2.0f;
    if (platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).state() == engine::platform::Key::State::Pressed) { speedMultiplier = 4.0f; }

    if (platform->key(engine::platform::KEY_W).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt * speedMultiplier); }
    if (platform->key(engine::platform::KEY_S).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt * speedMultiplier); }
    if (platform->key(engine::platform::KEY_A).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt * speedMultiplier); }
    if (platform->key(engine::platform::KEY_D).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt * speedMultiplier); }
    // Vertikalno kretanje
    if (platform->key(engine::platform::KEY_E).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::UP, dt * speedMultiplier); }
    if (platform->key(engine::platform::KEY_Q).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt * speedMultiplier); }

    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

}