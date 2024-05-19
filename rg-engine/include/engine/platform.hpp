//
// Created by oracle on 13.5.24..
//

#ifndef MATF_RG_PROJECT_PLATFORM_H
#define MATF_RG_PROJECT_PLATFORM_H

#include "GLFW/glfw3.h"
#include "engine/utils.hpp"
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace rg {

    class PlatformController : public Controller {
        friend class ControllerManager;

    private:
        static std::unique_ptr<PlatformController> create();
    };

    class WindowImpl;

    class WindowController : public Controller {
        friend class ControllerManager;

    public:
        int width() const {
            return m_width;
        }

        int height() const {
            return m_height;
        }

        const std::string &title() {
            return m_title;
        }

        const WindowImpl *handle() const {
            return m_window_impl;
        }

        std::string_view name() const override {
            return "WindowController";
        }

        ~WindowController() override;

    private:
        WindowImpl *m_window_impl = nullptr;
        int m_width = 0;
        int m_height = 0;
        std::string m_title;

        static std::unique_ptr<WindowController> create();

        bool initialize() override;

        void terminate() override;
    };

    enum KeyId {
        MOUSE_BUTTON_1 = 0,
        MOUSE_BUTTON_2,
        MOUSE_BUTTON_3,
        MOUSE_BUTTON_4,
        MOUSE_BUTTON_5,
        MOUSE_BUTTON_6,
        MOUSE_BUTTON_7,
        MOUSE_BUTTON_8,
        MOUSE_BUTTON_LAST,
        MOUSE_BUTTON_LEFT,
        MOUSE_BUTTON_RIGHT,
        MOUSE_BUTTON_MIDDLE,
        KEY_SPACE,
        KEY_APOSTROPHE,
        KEY_COMMA,
        KEY_MINUS,
        KEY_PERIOD,
        KEY_SLASH,
        KEY_0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,
        KEY_SEMICOLON,
        KEY_EQUAL,
        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,
        KEY_LEFT_BRACKET,
        KEY_BACKSLASH,
        KEY_RIGHT_BRACKET,
        KEY_GRAVE_ACCENT,
        KEY_WORLD_1,
        KEY_WORLD_2,
        KEY_ESCAPE,
        KEY_ENTER,
        KEY_TAB,
        KEY_BACKSPACE,
        KEY_INSERT,
        KEY_DELETE,
        KEY_RIGHT,
        KEY_LEFT,
        KEY_DOWN,
        KEY_UP,
        KEY_PAGE_UP,
        KEY_PAGE_DOWN,
        KEY_HOME,
        KEY_END,
        KEY_CAPS_LOCK,
        KEY_SCROLL_LOCK,
        KEY_NUM_LOCK,
        KEY_PRINT_SCREEN,
        KEY_PAUSE,
        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_F11,
        KEY_F12,
        KEY_F13,
        KEY_F14,
        KEY_F15,
        KEY_F16,
        KEY_F17,
        KEY_F18,
        KEY_F19,
        KEY_F20,
        KEY_F21,
        KEY_F22,
        KEY_F23,
        KEY_F24,
        KEY_F25,
        KEY_KP_0,
        KEY_KP_1,
        KEY_KP_2,
        KEY_KP_3,
        KEY_KP_4,
        KEY_KP_5,
        KEY_KP_6,
        KEY_KP_7,
        KEY_KP_8,
        KEY_KP_9,
        KEY_KP_DECIMAL,
        KEY_KP_DIVIDE,
        KEY_KP_MULTIPLY,
        KEY_KP_SUBTRACT,
        KEY_KP_ADD,
        KEY_KP_ENTER,
        KEY_KP_EQUAL,
        KEY_LEFT_SHIFT,
        KEY_LEFT_CONTROL,
        KEY_LEFT_ALT,
        KEY_LEFT_SUPER,
        KEY_RIGHT_SHIFT,
        KEY_RIGHT_CONTROL,
        KEY_RIGHT_ALT,
        KEY_RIGHT_SUPER,
        KEY_MENU,
        KEY_COUNT,
    };

    class Key {
        friend class InputController;

    public:
        enum class State { Released, JustPressed, Pressed, JustReleased };

        State &state() {
            return m_state;
        }

        KeyId key() const {
            return m_key;
        }

        State state() const {
            return m_state;
        }

        std::string_view to_string() const;

    private:
        KeyId m_key = KEY_COUNT;
        State m_state = State::Released;
    };

    struct MousePosition {
        double x;
        double y;
        double dx;
        double dy;
    };

    class InputController : public Controller {
        friend class ControllerManager;

    public:
        const Key &key(KeyId key) const;

        Key &key(KeyId key);

        std::string_view name() const override {
            return "InputController";
        }

        const MousePosition &mouse() const {
            return m_mouse;
        }

    private:
        static std::unique_ptr<InputController> create();

        bool initialize() override;

        void update() override;

        void update_key(Key &key_data);

        void update_mouse();

        std::vector<Key> m_keys;

        MousePosition m_mouse;
    };


}// namespace rg

#endif//MATF_RG_PROJECT_PLATFORM_H
