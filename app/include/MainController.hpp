#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/resources/Shader.hpp>
#include <engine/resources/Skybox.hpp>
#include <engine/resources/Model.hpp>

namespace app {

class MainController : public engine::core::Controller {
public:
    std::string_view name() const override { return "MainController"; }

private:
    // Shaders & skybox
    engine::resources::Shader *m_skyboxShader{nullptr};
    engine::resources::Skybox *m_skybox{nullptr};
    engine::resources::Shader *m_basicShader{nullptr};

    // Models
    engine::resources::Model *m_tree{nullptr};
    engine::resources::Model *m_campfire{nullptr};
    engine::resources::Model *m_tent{nullptr};
    engine::resources::Model *m_stone{nullptr};
    engine::resources::Model *m_lantern{nullptr};
    engine::resources::Model *m_deadtrunk{nullptr};

    float m_moveSpeed = 3.0f;
    float m_sprintScale = 2.0f;

    // lifecycle
    void initialize() override;

    bool loop() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void update_camera();

    void update_lighting_adjustments();

    void update_event_sequence();
};

}// namespace app

#endif // MAINCONTROLLER_HPP
