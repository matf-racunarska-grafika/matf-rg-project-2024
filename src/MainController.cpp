#include <memory>
#include <spdlog/spdlog.h>
#include <rg/Engine.hpp>
#include <rg/graphics/GraphicsController.hpp>
#include <app/MainController.hpp>
#include <app/GUIController.hpp>

namespace app {
    void MainPlatformEventObserver::on_keyboard(rg::platform::Key key) {
        spdlog::info("Keyboard event: key={}, state=", static_cast<int>(key.id()), to_string(key.state()));
    }

    void MainController::initialize() {
        // User initialization
        rg::graphics::OpenGL::enable_depth_testing();

        auto observer = std::make_unique<MainPlatformEventObserver>();
        rg::controller::get<rg::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
    }

    bool MainController::loop() {
        const auto platform = rg::controller::get<rg::platform::PlatformController>();
        if (platform->key(rg::platform::KeyId::KEY_ESCAPE).state() == rg::platform::Key::State::JustPressed) {
            return false;
        }
        return true;
    }

    void MainController::poll_events() {
        const auto platform = rg::controller::get<rg::platform::PlatformController>();
        if (platform->key(rg::platform::KEY_F1).state() == rg::platform::Key::State::JustPressed) {
            m_cursor_enabled = !m_cursor_enabled;
            platform->set_enable_cursor(m_cursor_enabled);
        }
    }

    void MainController::update() {
        update_camera();
    }

    void MainController::begin_draw() {
        rg::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw() {
        draw_backpack();
        draw_skybox();
    }

    void MainController::end_draw() {
        rg::controller::get<rg::platform::PlatformController>()->swap_buffers();
    }

    void MainController::draw_backpack() {
        auto graphics = rg::controller::get<rg::graphics::GraphicsController>();
        auto shader   = rg::controller::get<rg::resources::ResourcesController>()->shader("basic");
        auto backpack = rg::controller::get<rg::resources::ResourcesController>()->model("backpack");
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_mat4("model", scale(glm::mat4(1.0f), glm::vec3(m_backpack_scale)));
        backpack->draw(shader);
    }

    void MainController::draw_skybox() {
        auto shader      = rg::controller::get<rg::resources::ResourcesController>()->shader("skybox");
        auto skybox_cube = rg::controller::get<rg::resources::ResourcesController>()->skybox("skybox");
        rg::controller::get<rg::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
    }

    void MainController::update_camera() {
        auto gui = rg::controller::get<GUIController>();
        if (gui->is_enabled()) {
            return;
        }
        auto platform = rg::controller::get<rg::platform::PlatformController>();
        auto camera   = rg::controller::get<rg::graphics::GraphicsController>()->camera();
        float dt      = platform->dt();
        if (platform->key(rg::platform::KEY_W).state() == rg::platform::Key::State::Pressed) {
            camera->process_keyboard(rg::graphics::Camera::Movement::FORWARD, dt);
        }
        if (platform->key(rg::platform::KEY_S).state() == rg::platform::Key::State::Pressed) {
            camera->process_keyboard(rg::graphics::Camera::Movement::BACKWARD, dt);
        }
        if (platform->key(rg::platform::KEY_A).state() == rg::platform::Key::State::Pressed) {
            camera->process_keyboard(rg::graphics::Camera::Movement::LEFT, dt);
        }
        if (platform->key(rg::platform::KEY_D).state() == rg::platform::Key::State::Pressed) {
            camera->process_keyboard(rg::graphics::Camera::Movement::RIGHT, dt);
        }
        auto mouse = platform->mouse();
        camera->process_mouse_movement(mouse.dx, mouse.dy);
        camera->process_mouse_scroll(mouse.scroll);
    }
}



