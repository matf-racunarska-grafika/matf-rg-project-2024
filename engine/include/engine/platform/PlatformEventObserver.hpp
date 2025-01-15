/**
 * @file PlatformEventObserver.hpp
 * @brief Defines the PlatformEventObserver class that serves as the interface for platform-specific event observers.
 */

#ifndef PLATFORMEVENTOBSERVER_HPP
#define PLATFORMEVENTOBSERVER_HPP
#include <engine/platform/Input.hpp>

namespace engine::platform {
    /**
    * @class PlatformEventObserver
    * @brief Platform events callback object.
    * Extend this class and override the methods you want to be called by the @ref PlatformController, then
    * register an instance with @ref PlatformController::register_observer.
    */
    class PlatformEventObserver {
    public:
        /**
        * @brief Called by @ref PlatformController for every frame in which the mouse moved.
        */
        virtual void on_mouse_move(MousePosition position) { }

        /**
        * @brief Called by @ref PlatformController for every frame in an event occured on the keyboard or mouse key.
        */
        virtual void on_key(Key key) { }

        /**
        * @brief Called by @ref PlatformController when the window is resized.
        */
        virtual void on_window_resize(int width, int height) { }

        virtual ~PlatformEventObserver() = default;
    };
}
#endif //PLATFORMEVENTOBSERVER_HPP
