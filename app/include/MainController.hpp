#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/core/Engine.hpp>

namespace app {
    class MainController final : public engine::core::Controller {
        void initialize() override;

        bool loop() override;

        void update_camera();

        void update() override;

        void update_skull_facing();
        void update_game_state();

        void draw() override;

        void draw_skull();
        void draw_bridge();
        void draw_arena();
        void draw_eyes(glm::mat4 eye1Model, glm::mat4 eye2Model);
        void draw_filters();
        void draw_skybox();
        void draw_health_bar();

    public:
        std::string_view name() const override { return "app::MainController"; }
    };
}
#endif //MAINCONTROLLER_HPP
