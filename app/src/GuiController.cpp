//
// Created by Jovana on 14.7.2025..
//

#include <GuiController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <imgui.h>

namespace app{

void GUIController::initialize(){
    set_enable(false);
}

void GUIController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_F2)
                .state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
                }
}

void GUIController::draw(){
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    graphics->begin_gui();

    /* auto temple  = engine::core::Controller::get<engine::resources::ResourcesController>()->model("temple");
    static float f = 0.0f;
    ImGui::Begin(temple->name().c_str());
    ImGui::Text("Loaded from: %s", temple->path().c_str());
    ImGui::DragFloat("Temple scale", , 0.05, 0.1, 4.0);
    ImGui::End();*/

    // Draw camera info
    ImGui::Begin("Camera info");
    const auto &c = *camera;
    ImGui::Text("Camera position: (%f, %f, %f)", c.Position
                                                  .x, c.Position
                                                       .y, c.Position
                                                            .z);
    ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
    ImGui::Text("Camera front: (%f, %f, %f)", c.Front
                                               .x, c.Front
                                                    .y, c.Front
                                                         .z);
    ImGui::End();
    graphics->end_gui();
 }

}