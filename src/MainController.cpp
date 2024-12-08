#include <imgui.h>
#include <memory>
#include <rg/Engine.hpp>
#include <rg/graphics/GraphicsController.hpp>

class MainPlatformEventObserver : public rg::platform::PlatformEventObserver {
public:
    void on_keyboard(rg::platform::Key key) override {
        spdlog::info("Keyboard event: key={}, state=", key, key.state());
    }
};

class MainController : public rg::controller::Controller {
protected:
    void initialize() override {
        // User initialization
        rg::graphics::OpenGL::enable_depth_testing();

        auto observer = std::make_unique<MainPlatformEventObserver>();
        rg::controller::get<rg::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
    }

    bool loop() override {
        const auto platform = rg::controller::get<rg::platform::PlatformController>();
        if (platform->key(rg::platform::KeyId::KEY_ESCAPE).state() == rg::platform::Key::State::JustPressed) {
            return false;
        }
        return true;
    }

    void poll_events() override {
        const auto platform = rg::controller::get<rg::platform::PlatformController>();
        if (platform->key(rg::platform::KeyId::KEY_F2).state() == rg::platform::Key::State::JustPressed) {
            m_draw_gui = !m_draw_gui;
        }
        if (platform->key(rg::platform::KEY_F1).state() == rg::platform::Key::State::JustPressed) {
            m_cursor_enabled = !m_cursor_enabled;
            platform->set_enable_cursor(m_cursor_enabled);
        }
    }

    void update() override {
        update_camera();
    }

    void draw() override {
        rg::graphics::OpenGL::clear_buffers();
        draw_backpack();
        draw_skybox();
        draw_gui();
        rg::controller::get<rg::platform::PlatformController>()->swap_buffers();
    }

private:
    void draw_skybox();

    void draw_backpack();

    void update_camera();

    void draw_gui();

    float m_backpack_scale{1.0f};
    bool m_draw_gui{false};
    bool m_cursor_enabled{true};
};

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

    // skybox cube
    rg::controller::get<rg::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

void MainController::update_camera() {
    if (m_draw_gui) {
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

void MainController::draw_gui() {
    if (!m_draw_gui) {
        return;
    }
    rg::controller::get<rg::graphics::GraphicsController>()->begin_gui();
    auto camera = rg::controller::get<rg::graphics::GraphicsController>()->camera();
    // Draw backpack scale slider window
    {
        auto backpack  = rg::controller::get<rg::resources::ResourcesController>()->model("backpack");
        static float f = 0.0f;
        ImGui::Begin(backpack->name().c_str());
        ImGui::Text("Loaded from: %s", backpack->path().c_str());
        ImGui::DragFloat("Backpack scale", &m_backpack_scale, 0.05, 0.1, 4.0);
        ImGui::End();
    }
    // Draw Camera Info window
    {
        ImGui::Begin("Camera info");
        const auto &c = *camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::End();
    }

    rg::controller::get<rg::graphics::GraphicsController>()->end_gui();
}

class MainApp final : public rg::core::App {
protected:
    void setup() override {
        auto main_controller = rg::controller::register_controller<MainController>();
        main_controller->after(rg::controller::get<rg::controller::EngineSentinelController>());
    }
};

namespace rg {
    std::unique_ptr<core::App> core::App::create() {
        return std::make_unique<MainApp>();
    }
}


