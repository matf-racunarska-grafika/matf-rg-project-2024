#include <GuiController.hpp>
#include "engine/graphics/GraphicsController.hpp"
#include "engine/platform/PlatformController.hpp"
#include <imgui.h>

namespace app {
void GUIController::initialize() {
    set_enable(true);
}

void GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_F2)
                .state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
    }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    graphics->begin_gui();

    ImGui::Begin("Tutorial");
    ImGui::Text("Welcome to the 'Forest ambience' tutorial!");
    ImGui::Text("Press F2 to toggle the GUI.\n");
    ImGui::Separator();

    ImGui::Text("\t\tNAVIGATING THE SCENE");
    ImGui::Text("To navigate through the scene, use the WASD or Arrows keys.\n");
    ImGui::Text("To rotate through the scene, use the mouse.\n");
    ImGui::Separator();

    ImGui::Text("\t\tCHANGING LIGHTING");
    ImGui::Text(
            "To change lighting of the moon, press 1 to put it in the base color (white), and 2 to put it in the blood moon (bright red).\n");
    ImGui::Text(
            "To change the lighting of the street lamp light bulbs, press 3 to put them in the base warm yellow color, press4 to put them in the LED white color,\n"
            "and press 5 to completely turn off the lights.\n");
    ImGui::Separator();

    ImGui::Text("\t\tOTHER");
    ImGui::Text("Press ESC to exit the application.\n");
    //ImGui::Text("Camera position: (%f, %f, %f)", camera->Position.x, camera->Position.y, camera->Position.z);

    ImGui::End();

    graphics->end_gui();
}

} // app