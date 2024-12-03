//
// Created by spaske on 12/3/24.
//
#include <PlatformEventObserverImpl.hpp>

void PlatformEventObserverImpl::on_mouse(rg::MousePosition mouse) {
    m_camera->process_mouse_movement(mouse.dx, mouse.dy);
    m_camera->process_mouse_scroll(mouse.scroll);
}

void PlatformEventObserverImpl::on_keyboard(rg::Key key) {
    if (key.is_down(rg::KEY_F1)) {
        m_cursor_enabled = !m_cursor_enabled;
        m_platform_controller->set_enable_cursor(m_cursor_enabled);
    }
}

rg::CameraMovement PlatformEventObserverImpl::key_to_camera_movement(const rg::Key key) {
    if (key.state() == rg::Key::State::JustPressed) {
        switch (key.key()) {
        case rg::KeyId::KEY_W: return rg::CameraMovement::FORWARD;
        case rg::KeyId::KEY_S: return rg::CameraMovement::BACKWARD;
        case rg::KeyId::KEY_A: return rg::CameraMovement::LEFT;
        case rg::KeyId::KEY_D: return rg::CameraMovement::RIGHT;
        default: return rg::CameraMovement::None;
        }
    }
    return rg::CameraMovement::None;
}
