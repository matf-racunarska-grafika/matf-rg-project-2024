#ifndef MYAPP_GUICONTROLLER_HPP
#define MYAPP_GUICONTROLLER_HPP

#include <string_view>
#include <engine/core/Controller.hpp>

namespace engine::myapp {

class GUIController final : public engine::core::Controller {
public:
    std::string_view name() const override {
        return "myapp::GUIController";
    }

private:
    void initialize() override;
    void poll_events() override;
    void draw() override;
};

}

#endif // MYAPP_GUICONTROLLER_HPP
