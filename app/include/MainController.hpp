//
// Created by masa on 29/01/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

    class MainController : public engine::core::Controller {

        void initialize() override;
        bool loop() override;
        void update_camera();
        void update() override;
        void draw_terrain();
        void draw_monument();
        void draw_gazebo();
        void draw_columns();
        void draw_skybox();
        void draw_butterfly();
        void draw_butterfly_instanced();
        void draw_garden();
        void draw_statue();
        void draw_temple();
        void draw_cottage();
        void draw_tree();
        void draw() override;
        void begin_draw() override;
        void end_draw() override;
    public:
        std::string_view name() const override {
            return "app::MainController";
        }
    };

} // namespace app

#endif // MAINCONTROLLER_HPP
