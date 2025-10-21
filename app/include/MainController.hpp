#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/resources/Shader.hpp>
#include <engine/resources/Skybox.hpp>

namespace app {

class MainController : public engine::core::Controller {
public:
    std::string_view name() const override { return "MainController"; }

private:
    // --- Skybox podaci ---
    engine::resources::Shader *m_skyboxShader{nullptr};
    engine::resources::Skybox *m_skybox{nullptr};

    // --- Glavne funkcije kontrolera ---
    void initialize() override;

    bool loop() override;

    void update() override;

    void begin_draw() override;

    void draw() override;// <== DODATO
    void end_draw() override;

    void update_camera();
};

}// namespace app

#endif // MAINCONTROLLER_HPP
