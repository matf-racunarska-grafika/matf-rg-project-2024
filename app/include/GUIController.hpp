//
// Created by zesla on 10/25/25.
//

#ifndef MATF_RG_PROJECT_GUICONTROLLER_HPP
#define MATF_RG_PROJECT_GUICONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {
class GUIController : public engine::core::Controller {

public:
    std::string_view name() const override;

private:
    void initialize() override;
    void draw() override;
    void poll_events() override;
};

}
#endif//MATF_RG_PROJECT_GUICONTROLLER_HPP
