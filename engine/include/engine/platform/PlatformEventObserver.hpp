//
// Created by spaske on 12/7/24.
//

#ifndef PLATFORMEVENTOBSERVER_HPP
#define PLATFORMEVENTOBSERVER_HPP
#include <engine/platform/Input.hpp>

namespace engine::platform {
    class PlatformEventObserver {
    public:
        virtual void on_mouse_move(MousePosition position);

        virtual void on_key(Key key);

        virtual void on_window_resize(int width, int height);

        virtual ~PlatformEventObserver() = default;
    };
}
#endif //PLATFORMEVENTOBSERVER_HPP
