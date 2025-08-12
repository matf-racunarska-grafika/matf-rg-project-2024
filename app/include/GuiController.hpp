//
// Created by aleksa on 12.8.25.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class GUIController final : public engine::core::Controller {
public:
    std::string_view name() const override { return "test::app::GUIController"; }

private:
    void initialize() override;

    void poll_events() override;

    void draw() override;

};

}// app

#endif //GUICONTROLLER_HPP
