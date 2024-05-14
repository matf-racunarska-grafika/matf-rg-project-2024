//
// Created by oracle on 13.5.24..
//

#include "engine/platform.hpp"
#include "engine/utils.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

namespace rg::platform {

    static std::array<int, KEY_COUNT> g_engine_to_glfw_key;
    static std::array<Key, GLFW_KEY_LAST + 1> g_glfw_to_engine_key;

    void initialize_key_maps();

    struct WindowImpl {
        GLFWwindow *window;
    };

    std::string_view to_string(KeyState key_state) {
        switch (key_state) {
        case KeyState::Released: return "Released";
        case KeyState::JustPressed: return "JustPressed";
        case KeyState::Pressed: return "Pressed";
        case KeyState::JustReleased: return "JustReleased";
        default: return "UNIMPLEMENTED";
        }
    }

    Window::~Window() {
        delete m_window_impl;
        m_window_impl = nullptr;
    }

    std::unique_ptr<Window> Window::create() {
        auto result = std::make_unique<Window>();
        result->m_window_impl = new WindowImpl;
        return result;
    }

    bool Window::initialize() {
        m_width = 800;
        m_height = 600;
        m_title = "title";
        rg::utils::EngineError::guarantee(m_window_impl != nullptr, "Must instantiate m_window_impl first");
        m_window_impl->window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_window_impl->window) {
            spdlog::error("Failed to create a GLFW window!");
            return false;
        }
        glfwMakeContextCurrent(m_window_impl->window);
        return true;
    }

    void Window::terminate() {
        glfwDestroyWindow(m_window_impl->window);
    }

    std::string_view Window::name() {
        return "Window";
    }

    std::unique_ptr<InputController> InputController::create() {
        return std::make_unique<InputController>();
    }

    bool InputController::initialize() {
        glfwSetKeyCallback(utils::ServiceProvider::get<Window>()->handle()->window, glfw_key_callback);
        initialize_key_maps();
        m_keys.resize(KEY_COUNT + 1);
        for (int key = 0; key < m_keys.size(); ++key) {
            m_keys[key].key = key;
        }
        return true;
    }

    KeyData &InputController::key(Key key) {
        utils::EngineError::guarantee(key >= 0 && key < m_keys.size(), "Key out of bounds!");
        return m_keys[key];
    }

    const KeyData &InputController::key(Key key) const {
        utils::EngineError::guarantee(key >= 0 && key < m_keys.size(), "Key out of bounds!");
        return m_keys[key];
    }

    void update_key(KeyData &key_data, GLFWwindow *window) {
        int glfw_key_code = g_engine_to_glfw_key[key_data.key];
        int action = glfwGetKey(window, glfw_key_code);
        switch (key_data.state) {
        case rg::platform::KeyState::Released: {
            if (action == GLFW_PRESS) {
                key_data.state = KeyState::JustPressed;
            }
            break;
        }
        case rg::platform::KeyState::JustReleased: {
            if (action == GLFW_PRESS) {
                key_data.state = rg::platform::KeyState::JustPressed;
            } else if (action == GLFW_RELEASE) {
                key_data.state = KeyState::Released;
            }
            break;
        }
        case rg::platform::KeyState::JustPressed: {
            if (action == GLFW_RELEASE) {
                key_data.state = KeyState::JustReleased;
            } else if (action == GLFW_PRESS) {
                key_data.state = KeyState::Pressed;
            }
            break;
        }
        case rg::platform::KeyState::Pressed: {
            if (action == GLFW_RELEASE) {
                key_data.state = rg::platform::KeyState::JustReleased;
            }
            break;
        }
        }
    }

    void InputController::update() {
        auto window = utils::ServiceProvider::get<Window>()->handle()->window;
        for (int i = 0; i < KEY_COUNT; ++i) {
            update_key(key(static_cast<Key>(i)), window);
        }
    }

    std::string_view InputController::name() {
        return "InputController";
    }

    class PlatformGLFW3 : public Platform {
    public:
        bool initialize() override {
            if (!glfwInit()) {
                return false;
            }
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            return true;
        }

        void terminate() override {
            glfwTerminate();
        }

        void poll_events() override {
            glfwPollEvents();
        }

        std::string_view name() override {
            return "PlatformGLFW3";
        }
    };

    std::unique_ptr<Platform> Platform::create() {
        return std::make_unique<PlatformGLFW3>();
    }

    void initialize_key_maps() {
#include "glfw_key_mapping.h"
    }
}

static void glfw_key_callback(GLFWwindow *, int key, int, int action, int) {
    auto input = rg::utils::ServiceProvider::get<rg::platform::InputController>();
    auto &key_data = input->key(rg::platform::g_glfw_to_engine_key[key]);
    switch (key_data.state) {
    case rg::platform::KeyState::Released:
    case rg::platform::KeyState::JustReleased: {
        if (action == GLFW_PRESS) {
            key_data.state = rg::platform::KeyState::JustPressed;
        }
        break;
    }
    case rg::platform::KeyState::JustPressed:
    case rg::platform::KeyState::Pressed: {
        if (action == GLFW_RELEASE) {
            key_data.state = rg::platform::KeyState::JustReleased;
        }
        break;
    }
    }
}

