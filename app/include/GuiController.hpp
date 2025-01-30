//
// Created by masa on 30/01/25.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class GUIController : public engine::core::Controller{
public:
    std::string_view name() const override {
        return "app::GUIController";
    }
private:
    void initialize() override;
    void draw() override;
    void poll_events() override;
};

} // app

#endif //GUICONTROLLER_HPP
