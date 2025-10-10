
#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP

#include <engine/core/Engine.hpp>

namespace app {
class GUIController final : public engine::core::Controller {
public:
    std::string_view name() const override {
        return "test::app::GUIController";
    }

    float pointLightIntensity = 1.0f;

private:
    void initialize() override;

    void poll_events() override;

    void draw() override;
};
}
#endif //GUICONTROLLER_HPP
