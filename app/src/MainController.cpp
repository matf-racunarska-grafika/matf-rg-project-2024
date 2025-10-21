#include "MainController.hpp"

#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/Camera.hpp>

#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override {
        auto gfx = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = gfx->camera();
        camera->rotate_camera(position.dx, position.dy);
    }

    void on_scroll(engine::platform::MousePosition position) override {
        auto gfx = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = gfx->camera();


        camera->zoom(position.scroll);
        gfx->perspective_params().FOV = glm::radians(camera->Zoom);
    }

    void on_window_resize(int width, int height) override {
        auto gfx = engine::core::Controller::get<engine::graphics::GraphicsController>();
        gfx->perspective_params().Width = static_cast<float>(width);
        gfx->perspective_params().Height = static_cast<float>(height);
    }
};

void MainController::initialize() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());

    engine::graphics::OpenGL::enable_depth_testing();

    auto gfx = engine::core::Controller::get<engine::graphics::GraphicsController>();
    gfx->perspective_params().FOV = glm::radians(gfx->camera()->Zoom);

    spdlog::info("MainController initialized");
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) return false;
    return true;
}

void MainController::update() { update_camera(); }

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw() {
    /* empty for now */
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MainController::update_camera() {
    auto gfx = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    auto camera = gfx->camera();

    float dt = platform->dt();

    using M = engine::graphics::Camera::Movement;
    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) camera->move_camera(M::FORWARD, dt);
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) camera->move_camera(M::BACKWARD, dt);
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) camera->move_camera(M::LEFT, dt);
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) camera->move_camera(M::RIGHT, dt);
    if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) camera->move_camera(M::UP, dt);
    if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) camera->move_camera(M::DOWN, dt);

    // uzimamo miš iz platforme (objekat, ne pointer)
    const auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

}// namespace app
