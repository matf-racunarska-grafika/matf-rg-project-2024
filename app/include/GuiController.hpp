//
// Created by filipn on 9/2/25.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/vec3.hpp>

namespace app {

class GuiController : public engine::core::Controller {
public:
    std::string_view name() const override { return "GuiController"; }

    glm::vec3 get_lamp_color() { return glm::vec3(lampColor[0], lampColor[1], lampColor[2]); }

private:
    void initialize() override;

    void poll_events() override;

    void draw() override;

    float lampColor[3] = {1.0f, 1.0f, 1.0f};
};

}// app

#endif //GUICONTROLLER_HPP
