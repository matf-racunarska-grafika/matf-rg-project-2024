//
// Created by spaske on 12/7/24.
//

#ifndef PLATFORMEVENTOBSERVER_HPP
#define PLATFORMEVENTOBSERVER_HPP
#include <rg/platform/Input.hpp>

namespace rg::platform {
    class PlatformEventObserver {
    public:
        virtual void on_mouse(MousePosition position);

        virtual void on_keyboard(Key key);

        virtual void on_window_resize(int width, int height);

        virtual ~PlatformEventObserver() = default;
    };
}
#endif //PLATFORMEVENTOBSERVER_HPP
