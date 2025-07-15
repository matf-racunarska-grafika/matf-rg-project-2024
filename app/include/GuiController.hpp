//
// Created by Jovana on 14.7.2025..
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

namespace app {

class GUIController : public engine::core::Controller {
public:
    float get_point_light_intensity() const { return pointLightIntensity; }
    float get_dir_light_intensity() const { return dirLightIntensity; }
    glm::vec3 get_point_light_color() const { return pointLightColor; }

    std::string_view name() const override { return "app::GUIController"; }

private:
    float pointLightIntensity = 1.0f;
    float dirLightIntensity = 1.5f;
    glm::vec3 pointLightColor = glm::vec3(1.0f, 0.6f, 0.4f);  // svetlo-roze-narandžasto

    void initialize() override;
    void draw() override;
    void poll_events() override;
};

} // namespace app

#endif // GUICONTROLLER_HPP
