//
// Created by matfrg on 10/13/25.
//

#include "AppUtils.hpp"

#include "GuiController.hpp"

#include <engine/core/Controller.hpp>

namespace app {

bool is_gui_active() {
    auto gui = engine::core::Controller::get<app::GUIController>();
    return gui->is_enabled();
}


} // app