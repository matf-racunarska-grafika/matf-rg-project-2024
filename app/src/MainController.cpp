#include "MainController.h"
#include "engine/core/Controller.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"
#include "spdlog/spdlog.h"

namespace app {

// Klasa za praćenje miša
class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx, position.dy);
}

// ----------------------
void MainController::initialize() {
    spdlog::info("MainController initialized");

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());

    engine::graphics::OpenGL::enable_depth_testing();

    // Podesi kameru kao u projektu 'grafika' (pozicija i orijentacija)
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto cam = graphics->camera();
    cam->Position = glm::vec3(-33.941f, 18.9701f, -10.2835f);
    cam->Yaw = 22.06f;
    cam->Pitch = 13.39f;
    cam->rotate_camera(0.0f, 0.0f);

    // Inicijalizuj skybox shader (postavi sampler)
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skyboxShader = resources->shader("skybox");
    skyboxShader->use();
    skyboxShader->set_int("skybox", 0);
}

// ----------------------
bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        spdlog::info("Escape pressed — exiting app.");
        return false;
    }

    return true;
}

// ----------------------
void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

// Crtanje meseca
void MainController::draw_moon() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    engine::resources::Model* moon = resources->model("moon");
    auto shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    // Pozicioniranje kao u projektu 'grafika'
    model = glm::translate(model, glm::vec3(8.0f, 8.0f, 8.0f));
    model = glm::scale(model, glm::vec3(8.25f));

    shader->set_mat4("model", model);
    moon->draw(shader);
}

// Crtanje tulipa
void MainController::draw_tulip() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    engine::resources::Model* tulip = resources->model("tulip");
    auto shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    // Pozicioniranje kao u projektu 'grafika'
    model = glm::translate(model, glm::vec3(0.0f, 19.0f, 9.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.37f));

    shader->set_mat4("model", model);
    tulip->draw(shader);
}

// Glavna draw funkcija
void MainController::draw() {
    draw_moon();
    draw_tulip();

    // Nacrtaj skybox u pozadini
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    graphics->draw_skybox(resources->shader("skybox"), resources->skybox("skybox1"));
}

// ----------------------
void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

// ----------------------
void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KEY_W).state() == engine::platform::Key::State::Pressed)
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    if (platform->key(engine::platform::KEY_S).state() == engine::platform::Key::State::Pressed)
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    if (platform->key(engine::platform::KEY_A).state() == engine::platform::Key::State::Pressed)
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    if (platform->key(engine::platform::KEY_D).state() == engine::platform::Key::State::Pressed)
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);

    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

void MainController::update() {
    update_camera();
}

} // namespace app
