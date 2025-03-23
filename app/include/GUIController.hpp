//
// Created by strahinjas on 3/23/25.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace my_project {
class GUIController final : public engine::core::Controller {
public:
    std::string_view name() const override { return "my_project::GUIController"; }

private:
    void initialize() override;

    void poll_events() override;

    void draw() override;
};
}

#endif //GUICONTROLLER_HPP
