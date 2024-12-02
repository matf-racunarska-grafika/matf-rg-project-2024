//
// Created by spaske on 12/2/24.
//

#ifndef APPSTATECONTROLLER_HPP
#define APPSTATECONTROLLER_HPP
#include <engine/controller/Controller.hpp>
#include <engine/platform/Platform.hpp>
#include <engine/render/Camera.hpp>

class AppStateController final : public rg::Controller {
public:

    static std::unique_ptr<AppStateController> create();

    std::string_view name() const override { return "AppStateController";}

    rg::Camera *camera() {
        return &m_camera;
    }

    const glm::mat4& projection() const {
        return m_projection;
    }

private:
    void initialize() override;

    void terminate() override;

    bool loop() override;

    void update() override;

    void poll_events() override;

    void draw() override;

    void update_camera();

    glm::mat4 m_projection{};
    rg::Camera m_camera;
    rg::PlatformController * m_platform_controller{};
};

#endif //APPSTATECONTROLLER_HPP
