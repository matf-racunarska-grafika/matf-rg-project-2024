//
// Created by spaske on 12/3/24.
//

#ifndef PLATFORMEVENTOBSERVERIMPL_HPP
#define PLATFORMEVENTOBSERVERIMPL_HPP
#include <engine/platform/Platform.hpp>
#include <engine/resources/Camera.hpp>

class PlatformEventObserverImpl final : public rg::PlatformEventObserver {
public:
    explicit PlatformEventObserverImpl(rg::Camera *m_camera, rg::PlatformController *platform_controller) :
    m_camera(m_camera), m_platform_controller(platform_controller) {
    }

    void on_mouse(rg::MousePosition mouse) override;

    void on_keyboard(rg::Key key) override;

private:
    static rg::CameraMovement key_to_camera_movement(const rg::Key key);

    rg::Camera *m_camera;
    rg::PlatformController *m_platform_controller;
    bool m_cursor_enabled = false;
};

#endif //PLATFORMEVENTOBSERVERIMPL_HPP
