#include <memory>
#include <engine/core/Engine.hpp>

class StudentsApp : public rg::App {
protected:
    void initialize() override {
        // User initialization
        rg::OpenGL::enable_depth_testing();
    }

    bool loop() override {
        const auto platform = rg::controller<rg::PlatformController>();
        if (platform->key(rg::KeyId::KEY_ESCAPE).state() == rg::Key::State::JustPressed) {
            return false;
        }
        return true;
    }

    void poll_events() override {
        const auto platform = rg::controller<rg::PlatformController>();
        if (platform->key(rg::KeyId::KEY_F2).state() == rg::Key::State::JustPressed) {
            m_draw_gui = !m_draw_gui;
        }
        if (platform->key(rg::KEY_F1).state() == rg::Key::State::JustPressed) {
            m_cursor_enabled = !m_cursor_enabled;
            platform->set_enable_cursor(m_cursor_enabled);
        }
    }

    void begin_frame() override {
        auto platform = rg::controller<rg::PlatformController>();
        m_projection  = glm::perspective(glm::radians(45.0f),
                                        static_cast<float>(platform->window()->width()) / platform
                                        ->window()->
                                        height(),
                                        0.1f, 100.f);
        rg::OpenGL::clear_buffers();
    }

    void update() override {
        update_camera();
    }

    void draw() override {
        draw_backpack();
        draw_skybox();
        draw_gui();
    }

private:
    void draw_skybox();

    void draw_backpack();

    void update_camera();

    void draw_gui();

    glm::mat4 m_projection{};
    rg::Camera m_camera{glm::vec3(0.0f, 0.0f, 3.0f)};
    float m_backpack_scale{1.0f};
    bool m_draw_gui{false};
    bool m_cursor_enabled{true};
};

void StudentsApp::draw_backpack() {
    auto shader   = rg::controller<rg::ResourcesController>()->shader("basic");
    auto backpack = rg::controller<rg::ResourcesController>()->model("backpack");
    shader->use();
    shader->set_mat4("projection", m_projection);
    shader->set_mat4("view", m_camera.get_view_matrix());
    shader->set_mat4("model", scale(glm::mat4(1.0f), glm::vec3(m_backpack_scale)));
    backpack->draw(shader);
}

void StudentsApp::draw_skybox() {
    auto shader      = rg::controller<rg::ResourcesController>()->shader("skybox");
    auto skybox_cube = rg::controller<rg::ResourcesController>()->skybox("skybox");
    glm::mat4 view   = glm::mat4(glm::mat3(m_camera.get_view_matrix()));
    shader->use();
    shader->set_mat4("view", view);
    shader->set_mat4("projection", m_projection);
    // skybox cube
    rg::OpenGL::draw_skybox(skybox_cube);
}

void StudentsApp::update_camera() {
    if (m_draw_gui) {
        return;
    }
    auto platform = rg::controller<rg::PlatformController>();
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
    auto mouse = platform->mouse();
    m_camera.process_mouse_movement(mouse.dx, mouse.dy);
    m_camera.process_mouse_scroll(mouse.scroll);
}

void StudentsApp::draw_gui() {
    if (!m_draw_gui) {
        return;
    }

    // draw info
    {
        auto backpack = rg::controller<rg::ResourcesController>()->model("backpack"); {
            static float f = 0.0f;
            ImGui::Begin(backpack->name().c_str());
            ImGui::Text("Loaded from: %s", backpack->path().c_str());
            ImGui::DragFloat("Backpack scale", &m_backpack_scale, 0.05, 0.1, 4.0);
            ImGui::End();
        } {
            ImGui::Begin("Camera info");
            const auto &c = m_camera;
            ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
            ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
            ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
            ImGui::End();
        }
    }
}

int main(int argc, char **argv) {
    return std::make_unique<StudentsApp>()->run(argc, argv);
}
