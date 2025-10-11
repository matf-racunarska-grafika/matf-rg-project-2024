//
// Created by nikola on 10/8/25.
//

#include <MyController.hpp>

#include "GUIController.hpp"
#include "../../engine/libs/glfw/include/GLFW/glfw3.h"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/graphics/OpenGL.hpp"
#include "engine/platform/PlatformController.hpp"
#include "engine/resources/ResourcesController.hpp"

#define BUILDING_COUNT (10)
#define TIME_SWITCH_SECONDS (0.5)

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

    float angle = 0;
    auto scale = glm::vec3(1.0f);
    glm::vec3 rotation_axis = glm::vec3(0,1,0);
    auto diffuse = glm::vec3(0.6f);
    auto specular = glm::vec3(0.4f);
    glm::vec3 relative_position = glm::vec3(0.0f, 2.0f, 0.0f);
    float linear = 0.05f;
    float quadratic = 0.05f;
    float shininess = 32.0f;
    for (int i = 0; i < BUILDING_COUNT; i++) {
        glm::vec3 ambient;
        int rotation = i % 3;
        if (rotation == 0) ambient = glm::vec3(0.6f, 0.0f, 0.0f);
        else if (rotation == 1) ambient = glm::vec3(0.0f, 0.6f, 0.0f);
        else ambient = glm::vec3(0.0f, 0.0f, 0.6f);

        addPointLight(new PointLight("lamp", glm::vec3(i*5-0.825f,0.185f,0), scale, angle, rotation_axis, ambient, diffuse, specular, relative_position, linear, quadratic, shininess));
        addDrawable(new ModelDrawable("building",  glm::vec3(i*5,0,-1), glm::vec3(0.03)));
        addDrawable(new ModelDrawable("road",  glm::vec3(i*5+1.5,0,2), glm::vec3(0.2, 0.2, 0.5), 90, glm::vec3(0,1,0)));
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

void MyController::update_lights() {
    auto platform = get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_P).state() == engine::platform::Key::State::JustPressed) {
        for (auto point_light: m_point_lights) {
            point_light->enabled = !point_light->enabled;
        }
    }
    if (platform->key(engine::platform::KeyId::KEY_J).state() == engine::platform::Key::State::JustPressed) {
        m_light_switching = LEFT;
    }
    if (platform->key(engine::platform::KeyId::KEY_K).state() == engine::platform::Key::State::JustPressed) {
        m_light_switching = NO_SWITCH;
    }
    if (platform->key(engine::platform::KeyId::KEY_L).state() == engine::platform::Key::State::JustPressed) {
        m_light_switching = RIGHT;
    }
    auto current_time = glfwGetTime();
    if (m_light_switching != NO_SWITCH && m_next_switch < current_time) {
        m_next_switch = current_time + TIME_SWITCH_SECONDS;
        for (auto point_light: m_point_lights) {
            auto& amb = point_light->ambient;
            if (m_light_switching == LEFT) {
                point_light->ambient = glm::vec3(amb.z, amb.x, amb.y);
            }
            else {
                point_light->ambient = glm::vec3(amb.y, amb.z, amb.x);
            }
        }
    }
}

void MyController::update() {
    update_camera();
    update_lights();
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
        drawable->draw(directional_light, m_point_lights);
    }
    draw_skybox();
}
void MyController::end_draw() {
    get<engine::platform::PlatformController>()->swap_buffers();
}

void MyController::terminate() {
    for (auto drawable : m_drawables) {
        delete drawable;
    }
    //m_point_lights elemnts have already existed in m_drawables so no need to deallocate them
    m_drawables.clear();
    m_point_lights.clear();
}

void MyController::addDrawable(Drawable *drawable) {
    m_drawables.push_back(drawable);
}

void MyController::addPointLight(PointLight* point_light) {
    m_point_lights.push_back(point_light);
    m_drawables.push_back(point_light);
}
}// namespace app