//
// Created by bojana on 10/8/25.
//

#ifndef MATF_RG_PROJECT_MYCONTROLLER_HPP
#define MATF_RG_PROJECT_MYCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformEventObserver.hpp>

namespace app {

class MyController : public engine::core::Controller {
    void initialize() override;
    bool loop() override;
    void draw_backpack();
    void draw_skybox();
    void draw() override;

    void update_camera();
    void update() override;
    void begin_draw() override;
    void end_draw() override;

public:
    std::string_view name() const override {
        return "MyController";
    }
};

}// namespace app

#endif//MATF_RG_PROJECT_MYCONTROLLER_HPP
