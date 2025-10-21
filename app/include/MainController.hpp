//
// Created by zesla on 10/9/25.
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app{
class MainController: public engine::core::Controller {
    void initialize() override;
    bool loop() override;
    void draw_bed();
    void begin_draw() override;
    void update_camera();
    void update() override;
    void draw() override;
    void end_draw() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }

};

}
#endif//MATF_RG_PROJECT_MAINCONTROLLER_HPP
