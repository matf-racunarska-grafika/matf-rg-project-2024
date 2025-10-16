//
// Created by matfrg on 10/13/25.
//

#include "GuiController.hpp"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <imgui.h>

namespace app {

void GUIController::initialize() {
    set_enable(false);
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    camera->Position = glm::vec3(0.0f,0.5f,0.0f);
}
void GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if(platform->key(engine::platform::KeyId::KEY_F2).state()==engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
        if(is_enabled()) {
            platform->set_enable_cursor(true);
        }else {
            platform->set_enable_cursor(false);
        }
    }
}
void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    graphics->begin_gui();
    ImGui::SetNextWindowSize(ImVec2(300,200),ImGuiCond_Appearing);
    ImGui::Begin("Camera info");
    ImGui::Text("Pozicija kamere (%.2f,%.2f,%.2f)",camera->Position.x,camera->Position.y,camera->Position.z);

    ImGui::Text("Boja zvezde");
    if(m_main_controller) {
        glm::vec3& color = m_main_controller->get_star_color();
        float color_arr[3] = {color.r,color.g,color.b};
        if(ImGui::ColorEdit3("Star Color",color_arr)) {
            color = glm::vec3(color_arr[0],color_arr[1],color_arr[2]);
        }
    }

    ImGui::End();

    graphics->end_gui();

}

} // app