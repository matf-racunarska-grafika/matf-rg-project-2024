//
// Created by nikola on 10/8/25.
//

#include "../include/MyController.hpp"

#include "GUIController.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"

#define BUILDING_COUNT (5)

namespace app {
class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui = engine::core::Controller::get<GUIController>();
    if (!gui->is_enabled()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }
}


void MyController::initialize() {

    for (int i = 0; i < BUILDING_COUNT; i++) {
        addDrawable(new Drawable("building", "basic", glm::vec3(i*5,0,0), glm::vec3(0.03)));
    }

    auto platform = get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
}
bool MyController::loop() {
    auto platform = get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}
void MyController::update_camera() {
    auto gui_controller = get<GUIController>();
    if (!gui_controller->is_enabled()) {
        auto platform = get<engine::platform::PlatformController>();
        auto graphics = get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        auto dt = platform->dt();
        if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
        }
    }
}
void MyController::update() {
    update_camera();
}
void MyController::draw_skybox() {
    auto resources = get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("mountain_skybox");
    auto shader = resources->shader("skybox");
    auto graphics = get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}
void MyController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MyController::draw() {
    for (auto drawable: m_drawables) {
        drawable->draw(directional_light, point_light);
    }
    draw_skybox();
}
void MyController::end_draw() {
    get<engine::platform::PlatformController>()->swap_buffers();
}

MyController::~MyController() {
    for (auto drawable : m_drawables) {
        delete drawable;
    }
    m_drawables.clear();
}

void MyController::addDrawable(Drawable *drawable) {
    m_drawables.push_back(drawable);
}
}// namespace app