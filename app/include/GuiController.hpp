//
// Created by matfrg on 10/13/25.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP

#include "MainController.hpp"


#include <engine/core/Controller.hpp>

namespace app {

class GUIController : public engine::core::Controller {
public:
    std::string_view name() const override {
        return "app::GUIController";
    };
    void set_main_controller(app::MainController *mc) {
        m_main_controller = mc;
    }

private:
    void initialize() override;
    void poll_events() override;
    void draw() override;
    app::MainController *m_main_controller = nullptr;
};

}


#endif //GUICONTROLLER_HPP
