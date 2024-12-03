#include <memory>
#include <engine/Engine.hpp>
#include <PlatformEventObserverImpl.hpp>

class StudentsApp : public rg::App {
protected:
    void initialize(int argc, char **argv) override {
        rg::ArgParser::instance()->initialize(argc, argv);
        rg::Configuration::instance()->initialize();

        // register engine controller
        auto controller_manager = rg::ControllerManager::instance();
        auto platform           = controller_manager->register_controller<rg::PlatformController>();
        auto resources          = controller_manager->register_controller<rg::ResourcesController>();
        auto sentinel           = controller_manager->register_controller<rg::EngineControllersSentinel>();

        resources->after(platform);

        sentinel->after(platform);
        sentinel->after(resources);

        controller_manager->initialize();

        platform->register_platform_event_observer(
                std::make_unique<PlatformEventObserverImpl>(&m_camera, platform));

        // User initialization
        rg::OpenGL::enable_depth_testing();
    }

    bool loop() override {
        /*
         * Any controller can stop the rendering loop.
         */
        if (!rg::ControllerManager::instance()->loop()) {
            return false;
        }

        const auto input = rg::ControllerManager::get<rg::PlatformController>();
        if (input->key(rg::KeyId::KEY_ESCAPE).state() == rg::Key::State::JustPressed) {
            return false;
        }
        return true;
    }

    void poll_events() override {
        rg::ControllerManager::instance()->poll_events();
    }

    void begin_frame() override {
        rg::ControllerManager::instance()->begin_frame();
        auto platform = rg::ControllerManager::get<rg::PlatformController>();
        m_projection  = glm::perspective(glm::radians(45.0f),
                                        static_cast<float>(platform->window()->width()) / platform
                                        ->window()->
                                        height(),
                                        0.1f, 100.f);
        rg::OpenGL::clear_buffers();
    }

    void update() override {
        rg::ControllerManager::instance()->update();
        update_camera();
    }

    void draw() override {
        rg::ControllerManager::instance()->draw();
        draw_backpack();
        draw_skybox();
    }

    void terminate() override {
        rg::ControllerManager::instance()->terminate();
    }

    void end_frame() override {
        rg::ControllerManager::instance()->end_frame();
    }

private:
    void draw_skybox();

    void draw_backpack();

    void update_camera();

    glm::mat4 m_projection{};
    rg::Camera m_camera{glm::vec3(0.0f, 0.0f, 3.0f)};
};

void StudentsApp::draw_backpack() {
    auto shader   = rg::ControllerManager::get<rg::ResourcesController>()->shader("basic");
    auto backpack = rg::ControllerManager::get<rg::ResourcesController>()->model("backpack");
    shader->use();
    shader->set_mat4("projection", m_projection);
    shader->set_mat4("view", m_camera.get_view_matrix());
    shader->set_mat4("model", glm::mat4(1.0f));
    backpack->draw(shader);
}

void StudentsApp::draw_skybox() {
    auto shader      = rg::ControllerManager::get<rg::ResourcesController>()->shader("skybox");
    auto skybox_cube = rg::ControllerManager::get<rg::ResourcesController>()->skybox("skybox");
    glm::mat4 view   = glm::mat4(glm::mat3(m_camera.get_view_matrix()));
    shader->use();
    shader->set_mat4("view", view);
    shader->set_mat4("projection", m_projection);
    // skybox cube
    rg::OpenGL::draw_skybox(skybox_cube);
}

void StudentsApp::update_camera() {
    auto platform = rg::ControllerManager::get<rg::PlatformController>();
    float dt      = platform->dt();
    if (platform->key(rg::KEY_W).state() == rg::Key::State::Pressed) {
        m_camera.process_keyboard(rg::FORWARD, dt);
    }
    if (platform->key(rg::KEY_S).state() == rg::Key::State::Pressed) {
        m_camera.process_keyboard(rg::BACKWARD, dt);
    }
    if (platform->key(rg::KEY_A).state() == rg::Key::State::Pressed) {
        m_camera.process_keyboard(rg::LEFT, dt);
    }
    if (platform->key(rg::KEY_D).state() == rg::Key::State::Pressed) {
        m_camera.process_keyboard(rg::RIGHT, dt);
    }
}

namespace rg {
    std::unique_ptr<App> create_app() {
        return std::make_unique<StudentsApp>();
    }
} // namespace rg

int main(int argc, char **argv) {
    return rg::App::run(argc, argv);
}
