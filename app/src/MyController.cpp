//
// Created by bojana on 10/8/25.
//

#include "../include/MyController.hpp"

#include <GUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    if (!gui_controller->is_enabled()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx,position.dy);
    }
}

void MyController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    auto platform = get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
}
bool MyController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}
void MyController::draw_backpack() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = get<engine::graphics::GraphicsController>();
    engine::resources::Model* model = resources->model("backpack");

    engine::resources::Shader* shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection",graphics->projection_matrix());
    shader->set_mat4("view",graphics->camera()->view_matrix());
    shader->set_mat4("model",glm::mat4(1.0f));

    model->draw(shader);
}

void MyController::draw() {
    draw_backpack();
}
void MyController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    if (!gui_controller->is_enabled()) {
        auto platform = get<engine::platform::PlatformController>();
        auto graphics = get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();

        float dt = platform->dt();
        if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            camera->move_camera(engine::graphics::Camera::FORWARD,dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
            camera->move_camera(engine::graphics::Camera::LEFT,dt);

        }  if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
            camera->move_camera(engine::graphics::Camera::BACKWARD,dt);


        }  if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
            camera->move_camera(engine::graphics::Camera::RIGHT,dt);

        }
    }

}
void MyController::update() {
    update_camera();
}
void MyController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}
void MyController::end_draw() {
    auto platform = get<engine::platform::PlatformController>();
    platform->swap_buffers();
}




}// namespace app