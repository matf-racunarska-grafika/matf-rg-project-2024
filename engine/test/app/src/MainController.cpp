#include <memory>
#include <spdlog/spdlog.h>
#include <engine/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <app/MainController.hpp>
#include <app/GUIController.hpp>

namespace engine::test::app {
    void MainPlatformEventObserver::on_key(engine::platform::Key key) {
        spdlog::info("Keyboard event: key={}, state={}", key.name(), key.state_str());
    }

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        spdlog::info("MousePosition: {} {}", position.x, position.y);
    }

    void MainController::initialize() {
        // User initialization
        engine::graphics::OpenGL::enable_depth_testing();

        auto observer = std::make_unique<MainPlatformEventObserver>();
        engine::controller::get<engine::platform::PlatformController>()->register_platform_event_observer(
                std::move(observer));
    }

    bool MainController::loop() {
        const auto platform = engine::controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).state() == engine::platform::Key::State::JustPressed) {
            return false;
        }
        return true;
    }

    void MainController::poll_events() {
        const auto platform = engine::controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
            m_cursor_enabled = !m_cursor_enabled;
            platform->set_enable_cursor(m_cursor_enabled);
        }
    }

    void MainController::update() {
        update_camera();
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw() {
        draw_backpack();
        draw_skybox();
    }

    void MainController::end_draw() {
        engine::controller::get<engine::platform::PlatformController>()->swap_buffers();
    }

    void MainController::draw_backpack() {
        auto graphics = engine::controller::get<engine::graphics::GraphicsController>();
        auto shader   = engine::controller::get<engine::resources::ResourcesController>()->shader("basic");
        auto backpack = engine::controller::get<engine::resources::ResourcesController>()->model("backpack");
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("model", scale(glm::mat4(1.0f), glm::vec3(m_backpack_scale)));
        backpack->draw(shader);
    }

    void MainController::draw_skybox() {
        auto shader      = engine::controller::get<engine::resources::ResourcesController>()->shader("skybox");
        auto skybox_cube = engine::controller::get<engine::resources::ResourcesController>()->skybox("skybox");
        engine::controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
    }

    void MainController::update_camera() {
        auto gui = engine::controller::get<GUIController>();
        if (gui->is_enabled()) {
            return;
        }
        auto platform = engine::controller::get<engine::platform::PlatformController>();
        auto camera   = engine::controller::get<engine::graphics::GraphicsController>()->camera();
        float dt      = platform->dt();
        if (platform->key(engine::platform::KEY_W).state() == engine::platform::Key::State::Pressed) {
            camera->process_keyboard(engine::graphics::Camera::Movement::FORWARD, dt);
        }
        if (platform->key(engine::platform::KEY_S).state() == engine::platform::Key::State::Pressed) {
            camera->process_keyboard(engine::graphics::Camera::Movement::BACKWARD, dt);
        }
        if (platform->key(engine::platform::KEY_A).state() == engine::platform::Key::State::Pressed) {
            camera->process_keyboard(engine::graphics::Camera::Movement::LEFT, dt);
        }
        if (platform->key(engine::platform::KEY_D).state() == engine::platform::Key::State::Pressed) {
            camera->process_keyboard(engine::graphics::Camera::Movement::RIGHT, dt);
        }
        auto mouse = platform->mouse();
        camera->process_mouse_movement(mouse.dx, mouse.dy);
        camera->process_mouse_scroll(mouse.scroll);
    }
}



