#include <AppStateController.hpp>

#include "engine/controller/Controller.hpp"

#include <engine/Engine.hpp>
#include <engine/render/Camera.hpp>
#include <memory>
#include <OpenGLRenderer.hpp>
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
    void initialize(int argc, char** argv) override {
        rg::trace();
        rg::ArgParser::instance()->initialize(argc, argv);
        rg::Configuration::instance()->initialize();

        // register engine controller
        auto controller_manager = rg::ControllerManager::singleton();
        controller_manager->initialize();

        auto platform_controller = controller_manager->register_controller<rg::PlatformController>();
        auto shader_controller = controller_manager->register_controller<rg::ShaderController>();
        auto assets_controller = controller_manager->register_controller<rg::AssetsController>();

        platform_controller->before(shader_controller);
        assets_controller->after(shader_controller);
        assets_controller->after(platform_controller);

        auto sentinel = controller_manager->get<rg::EngineControllersSentinel>();
        sentinel->after(platform_controller);
        sentinel->after(assets_controller);
        sentinel->after(shader_controller);

        auto app_state_controller = controller_manager->register_controller<AppStateController>();
        app_state_controller->after(sentinel);

        /*
         * Controller initialization is done after user-defined App::initialize because
         * user can register custom services in App::initialize_controllers.
         */
        controller_manager->initialize_controllers();

        platform_controller->register_platform_event_observer(std::make_unique<PlatformEventObserver>(app_state_controller->camera(), platform_controller));

        // User initialization
        m_renderer = OpenGLRenderer::instance();
        m_renderer->initialize();

        m_shader = shader_controller->get("basic");
        m_model = assets_controller->model("backpack");
        rg::trace();
    }

    bool loop() override {
        /*
         * Any controller can stop the rendering loop.
         */
        if (!rg::ControllerManager::singleton()->loop()) {
            return false;
        }

        const auto input = rg::ControllerManager::get<rg::PlatformController>();
        if (input->key(rg::KeyId::KEY_ESCAPE).state() == rg::Key::State::JustPressed) {
            return false;
        }
        return true;
    }

    void poll_events() override {
        rg::ControllerManager::singleton()->poll_events();
        m_renderer->begin_frame();
    }

    void update() override {
        rg::ControllerManager::singleton()->update();

        auto app_state_controller = rg::ControllerManager::get<AppStateController>();
        m_shader->use();
        m_shader->set_mat4("projection", app_state_controller->projection());
        m_shader->set_mat4("view", app_state_controller->camera()->get_view_matrix());
        m_shader->set_mat4("model", glm::mat4(1.0f));
    }

    void draw() override {
        rg::ControllerManager::singleton()->draw();

        m_model->draw(m_shader);


        m_renderer->draw_skybox();
        m_renderer->end_frame();
    }

    void terminate() override {
        m_renderer->terminate();
        rg::ControllerManager::singleton()->terminate();
    }

private:
    OpenGLRenderer* m_renderer;
    rg::ShaderProgram *m_shader;
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