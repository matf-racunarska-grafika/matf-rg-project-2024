//
// Created by masa on 30/01/25.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>


namespace app {

class GUIController : public engine::core::Controller{
public:
    std::string_view name() const override {
        return "app::GUIController";
    }
    glm::vec3 baseColors[5];
    float brightness[5];
private:
    void initialize() override;
    void draw() override;
    void poll_events() override;
    void updateLampColors();
    bool m_enabled = false;

};

} // app

#endif //GUICONTROLLER_HPP
