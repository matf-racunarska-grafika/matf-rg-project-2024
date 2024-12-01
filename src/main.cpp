#include "engine/controller/Controller.hpp"
#include "engine/ecs/EntityController.hpp"
#include "glad/glad.h"

#include <engine/Engine.hpp>
#include <engine/render/Camera.hpp>
#include <memory>
#include <spdlog/spdlog.h>
/**
 * Student implements rg::App for their application.
 */

class PlatformEventObserver final : public rg::PlatformEventObserver {
public:
    explicit PlatformEventObserver(rg::Camera *m_camera, rg::PlatformController* platform_controller) : m_camera(m_camera), m_platform_controller(platform_controller) {
    }

    void on_mouse(rg::MousePosition mouse) override {
        m_camera->process_mouse_movement(mouse.dx, mouse.dy);
        m_camera->process_mouse_scroll(mouse.scroll);
    }

    void on_keyboard(rg::Key key) override {
       if (key.is_down(rg::KEY_F1)) {
           m_cursor_enabled = !m_cursor_enabled;
           m_platform_controller->set_enable_cursor(m_cursor_enabled);
       }
    }

private:
    static rg::CameraMovement key_to_camera_movement(const rg::Key key) {
        if (key.state() == rg::Key::State::JustPressed) {
            switch (key.key()) {
            case rg::KeyId::KEY_W: return rg::CameraMovement::FORWARD;
            case rg::KeyId::KEY_S: return rg::CameraMovement::BACKWARD;
            case rg::KeyId::KEY_A: return rg::CameraMovement::LEFT;
            case rg::KeyId::KEY_D: return rg::CameraMovement::RIGHT;
            default: return rg::CameraMovement::None;
            }
        }
        return rg::CameraMovement::None;
    }
    rg::Camera* m_camera;
    rg::PlatformController* m_platform_controller;
    bool m_cursor_enabled = false;
};

class StudentsApp : public rg::App {
protected:
    void initialize() override {

    }

    void after_initialize() override {
        auto entity_controller = rg::ControllerManager::get<rg::EntityController>();
        auto shader_controller = rg::ControllerManager::get<rg::ShaderController>();
        auto assets_controller = rg::ControllerManager::get<rg::AssetsController>();
        auto platform_controller = rg::ControllerManager::get<rg::PlatformController>();

        platform_controller->register_platform_event_observer(std::make_unique<PlatformEventObserver>(&m_camera, platform_controller));

        m_shader = shader_controller->get("basic");
        m_entity = entity_controller->create_entity();
        m_model = assets_controller->model("backpack").value();
        m_entity->add_component<rg::DrawableComponent>(m_model, m_shader);
    }

    bool loop() override {
        const auto input = rg::ControllerManager::get<rg::PlatformController>();
        if (input->key(rg::KeyId::KEY_ESCAPE).state() == rg::Key::State::JustPressed) {
            return false;
        }
        return true;
    }

    void poll_events() override {

    }

    void update() override {
        auto platform_controller = rg::ControllerManager::get<rg::PlatformController>();
        update_camera();
        m_shader->use();
        glm::mat4 projection = glm::perspective(glm::radians(m_camera.Zoom),
                                                static_cast<float>(platform_controller->window_width()) /
                                                        platform_controller->window_height(),
                                                0.1f, 100.0f);
        m_shader->set_mat4("projection", projection);
        m_shader->set_mat4("view", m_camera.get_view_matrix());
        m_shader->set_mat4("model", glm::mat4(1.0f));
    }

    void draw() override {
    }

    void terminate() override {
    }

private:

    void update_camera() {
        auto platform_controller = rg::ControllerManager::get<rg::PlatformController>();
        float dt = platform_controller->dt();
        if (platform_controller->key(rg::KEY_W).state() == rg::Key::State::Pressed) {
            m_camera.process_keyboard(rg::FORWARD, dt);
        }
        if (platform_controller->key(rg::KEY_S).state() == rg::Key::State::Pressed) {
            m_camera.process_keyboard(rg::BACKWARD, dt);
        }
        if (platform_controller->key(rg::KEY_A).state() == rg::Key::State::Pressed) {
            m_camera.process_keyboard(rg::LEFT, dt);
        }
        if (platform_controller->key(rg::KEY_D).state() == rg::Key::State::Pressed) {
            m_camera.process_keyboard(rg::RIGHT, dt);
        }

    }

    rg::Camera m_camera{glm::vec3(0.0f, 0.0f, 3.0f)};
    rg::ShaderProgram *m_shader;
    rg::Entity *m_entity;
    rg::Model *m_model;
};

namespace rg {
    std::unique_ptr<App> create_app() {
        return std::make_unique<StudentsApp>();
    }
}// namespace rg

int main(int argc, char **argv) {
    return rg::App::run(argc, argv);
}