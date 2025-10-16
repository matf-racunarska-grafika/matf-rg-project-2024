#include <memory>
#include <spdlog/spdlog.h>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <app/MainController.hpp>
#include <app/GUIController.hpp>

namespace engine::app {
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
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(
            std::move(observer));

    m_jellyFishPositions.clear();
    for (int i = 0; i < 20; i++) {
        float x = -5.0f + (float)(rand() % 1000) / 100.0f;
        float y = 0.5f + (float)(rand() % 150) / 100.0f;
        float z = -10.0f + (float)(rand() % 1000) / 100.0f;
        m_jellyFishPositions.push_back(glm::vec3(x, y, z));
    }
}

bool MainController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) {
        return false;
    }
    return true;
}

void MainController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1)
                .state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    }

    if (platform->key(engine::platform::KEY_L).state() == engine::platform::Key::State::JustPressed) {
        m_isDay = !m_isDay;
    }

    if (platform->key(engine::platform::KEY_B).state() == engine::platform::Key::State::JustPressed) {
        m_busMoving = !m_busMoving;
    }

}

void MainController::update() {
    update_camera();

    if (m_busMoving) {
        m_busOffset += m_busSpeed;
        if (m_busOffset >= m_busDistance) {
            m_busOffset = -5.0f;
        }
    }
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    draw_spongebob();
    draw_SBHouse();
    draw_gary();
    draw_bus();
    draw_busStop();
    draw_jellyFish();
    draw_ground();
    draw_skybox();
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}
    void MainController::draw_spongebob() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()
                      ->shader(m_isDay ? "basic" : "basic_point_light");
    auto spongebob = engine::core::Controller::get<engine::resources::ResourcesController>()->model("spongebob");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());


    if (m_isDay) {
        shader->set_vec3("lightDir", glm::vec3(0.5f, -1.0f, -0.5f));
        shader->set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    } else {
        shader->set_vec3("lightPos", glm::vec3(-3.0f, 0.0f, -2.0f));
        shader->set_vec3("lightColor", glm::vec3(2.0f, 1.6f, 1.2f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, -3.0f));
    model = glm::scale(model, glm::vec3(0.7f));

    shader->set_mat4("model", model);
    spongebob->draw(shader);
}

    void MainController::draw_SBHouse() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()
                      ->shader(m_isDay ? "basic" : "basic_point_light");
    auto house = engine::core::Controller::get<engine::resources::ResourcesController>()->model("SBHouse");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    if (m_isDay) {
        shader->set_vec3("lightDir", glm::vec3(0.5f, -1.0f, -0.5f));
        shader->set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    } else {
        shader->set_vec3("lightPos", glm::vec3(-3.0f, 0.0f, -2.0f));
        shader->set_vec3("lightColor", glm::vec3(2.0f, 1.6f, 1.2f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.5f, -0.6f, -6.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f));

    shader->set_mat4("model", model);
    house->draw(shader);
}

    void MainController::draw_gary() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()
                   ->shader(m_isDay ? "basic" : "basic_point_light");
    auto gary = engine::core::Controller::get<engine::resources::ResourcesController>()->model("gary");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    if (m_isDay) {
        shader->set_vec3("lightDir", glm::vec3(0.5f, -1.0f, -0.5f));
        shader->set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    } else {
        shader->set_vec3("lightPos", glm::vec3(-3.0f, 0.0f, -2.0f));
        shader->set_vec3("lightColor", glm::vec3(2.0f, 1.6f, 1.2f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.5f, -0.48f, -3.5f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.0015f));

    shader->set_mat4("model", model);
    gary->draw(shader);
}

    void MainController::draw_bus() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()
                   ->shader(m_isDay ? "basic" : "basic_point_light");
    auto bus = engine::core::Controller::get<engine::resources::ResourcesController>()->model("SBBus");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    if (m_isDay) {
        shader->set_vec3("lightDir", glm::vec3(0.5f, -1.0f, -0.5f));
        shader->set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    } else {
        shader->set_vec3("lightPos", glm::vec3(-3.0f, 0.0f, -2.0f));
        shader->set_vec3("lightColor", glm::vec3(2.0f, 1.6f, 1.2f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    }

    glm::mat4 model = glm::mat4(1.0f);
    float angleY = glm::radians(120.0f);
    glm::vec3 forwardDir = glm::normalize(glm::vec3(cos(angleY), 0.0f, -sin(angleY)));
    glm::vec3 pos = m_busStartPos - forwardDir * m_busOffset;

    model = glm::translate(model, pos);
    model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.25f));

    shader->set_mat4("model", model);
    bus->draw(shader);
}

    void MainController::draw_busStop() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()
                  ->shader(m_isDay ? "basic" : "basic_point_light");
    auto busStop = engine::core::Controller::get<engine::resources::ResourcesController>()->model("busStop");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    if (m_isDay) {
        shader->set_vec3("lightDir", glm::vec3(0.5f, -1.0f, -0.5f));
        shader->set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    } else {
        shader->set_vec3("lightPos", glm::vec3(-3.0f, 0.0f, -2.0f));
        shader->set_vec3("lightColor", glm::vec3(2.0f, 1.6f, 1.2f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    }

    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -0.5f, -2.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f));

    shader->set_mat4("model", model);
    busStop->draw(shader);
}

    void MainController::draw_jellyFish() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()
                      ->shader(m_isDay ? "basic" : "basic_point_light");
    auto jellyFish = engine::core::Controller::get<engine::resources::ResourcesController>()->model("jellyFish");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());


    if (m_isDay) {
        shader->set_vec3("lightDir", glm::vec3(0.5f, -1.0f, -0.5f));
        shader->set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    } else {
        shader->set_vec3("lightPos", glm::vec3(-3.0f, 0.0f, -2.0f));
        shader->set_vec3("lightColor", glm::vec3(2.0f, 1.6f, 1.2f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    }

    for (int i = 0; i < 20; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_jellyFishPositions[i]);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.0015f));

        shader->set_mat4("model", model);
        jellyFish->draw(shader);
    }
}

    void MainController::draw_ground() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()
                   ->shader(m_isDay ? "basic" : "basic_point_light");
    auto ground = engine::core::Controller::get<engine::resources::ResourcesController>()->model("ground");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    if (m_isDay) {
        shader->set_vec3("lightDir", glm::vec3(0.5f, -1.0f, -0.5f));
        shader->set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    } else {
        shader->set_vec3("lightPos", glm::vec3(-3.0f, 0.0f, -2.0f));
        shader->set_vec3("lightColor", glm::vec3(2.0f, 1.6f, 1.2f));
        shader->set_vec3("viewPos", graphics->camera()->Position);
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, -3.0f));
    model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));

    shader->set_mat4("model", model);
    ground->draw(shader);
}

void MainController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()
                            ->skybox(m_isDay ? "skybox_day" : "skybox_night");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

void MainController::update_camera() {
    auto gui = engine::core::Controller::get<GUIController>();
    if (gui->is_enabled()) {
        return;
    }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();
    if (platform->key(engine::platform::KEY_W)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }
    if (platform->key(engine::platform::KEY_S)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }
    if (platform->key(engine::platform::KEY_A)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }
    if (platform->key(engine::platform::KEY_D)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }
    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}
}