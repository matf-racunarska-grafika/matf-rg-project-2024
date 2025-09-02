//
// Created by filipn on 9/2/25.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class GuiController : public engine::core::Controller {
public:
    std::string_view name() const override { return "GuiController"; }

private:
    void initialize() override;

    void poll_events() override;

    void draw() override;
};

}// app

#endif //GUICONTROLLER_HPP
