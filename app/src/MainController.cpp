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

void MainController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("lighting");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_vec3("viewPos", graphics->camera()->Position);

    // Postavi globalne point lightove
    set_point_lights(shader);

    // Crtaj modele
    draw_backpack(glm::vec3(-5.0f, 0.0f, -7.0f));
    draw_backpack(glm::vec3(5.0f, 0.0f, -7.0f));

    draw_skybox();
}

void MainController::end_draw() { engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers(); }

// ---------------------------------------------------------------------------------------------------------------------------

void MainController::draw_light_source_cube(const glm::vec3 &lightPos, float scale) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto basicShader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    auto cubeModel = engine::core::Controller::get<engine::resources::ResourcesController>()->model("cube");

    basicShader->use();
    basicShader->set_mat4("projection", graphics->projection_matrix());
    basicShader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, lightPos);
    modelMat = glm::scale(modelMat, glm::vec3(scale));
    basicShader->set_mat4("model", modelMat);

    cubeModel->draw(basicShader);
}

void MainController::set_point_lights(auto shader) {
    // Postavljanje uniform-e za prvi point light
    shader->set_vec3("pointLights[0].position", glm::vec3(-5.0, 1.0f, 2.0f));
    shader->set_float("pointLights[0].constant", 1.0f);
    shader->set_float("pointLights[0].linear", 0.09f);
    shader->set_float("pointLights[0].quadratic", 0.032f);
    shader->set_vec3("pointLights[0].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    shader->set_vec3("pointLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader->set_vec3("pointLights[0].specular", glm::vec3(0.3f, 0.3f, 0.3f));

    // Postavljanje uniform-e za drugi point light
    shader->set_vec3("pointLights[1].position", glm::vec3(0.0f));// ili neka daleka pozicija
    shader->set_float("pointLights[1].constant", 1.0f);
    shader->set_float("pointLights[1].linear", 0.0f);
    shader->set_float("pointLights[1].quadratic", 0.0f);
    shader->set_vec3("pointLights[1].ambient", glm::vec3(0.0f));
    shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.0f));
    shader->set_vec3("pointLights[1].specular", glm::vec3(0.0f));

    // Cube na poziciji izvora point light-ova
    draw_light_source_cube(glm::vec3(1.2f, 1.0f, 2.0f), 0.5f); // prvi izvor svetla
    draw_light_source_cube(glm::vec3(-1.2f, 1.0f, 2.0f), 0.5f);// drugi izvor svetla
}

void MainController::draw_backpack(const glm::vec3 &modelPos) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("lighting");
    auto backpack = engine::core::Controller::get<engine::resources::ResourcesController>()->model("backpack");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, modelPos);
    model = glm::scale(model, glm::vec3(m_backpack_scale));
    shader->set_mat4("model", model);

    auto camera = graphics->camera();
    shader->set_vec3("viewPos", camera->Position);

    backpack->draw(shader);
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
    float speedMultiplier = 1.0f;
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