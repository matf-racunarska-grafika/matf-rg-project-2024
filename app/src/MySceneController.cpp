//
// Created by matija on 10/12/25.
//

#include "../include/MySceneController.hpp"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {

void MySceneController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    //auto observer = std::make_unique<MainPlatformEventObserver>();
    //engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
}

bool MySceneController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) {
        return false;
    }
    return true;
}

void MySceneController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1)
                .state() == engine::platform::Key::State::JustPressed) {
        //platform->set_enable_cursor(false);
    }
}

void MySceneController::update() {
}

void MySceneController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MySceneController::draw() {
    draw_skybox();
}

void MySceneController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

void MySceneController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

}



