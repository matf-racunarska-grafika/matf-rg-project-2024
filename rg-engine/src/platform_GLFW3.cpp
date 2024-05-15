//
// Created by oracle on 13.5.24..
//

#include "engine/platform.hpp"
#include "engine/utils.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

namespace rg {

    static std::array<int, KEY_COUNT> g_engine_to_glfw_key;
    static std::array<KeyId, KEY_COUNT> g_glfw_to_engine_key;

    void initialize_key_maps();

    struct WindowImpl {
        GLFWwindow *window;
    };

    std::string_view Key::to_string() const {
        switch (m_state) {
        case Key::State::Released: return "Released";
        case Key::State::JustPressed: return "JustPressed";
        case Key::State::Pressed: return "Pressed";
        case Key::State::JustReleased: return "JustReleased";
        default: return "UNIMPLEMENTED";
        }
    }

    WindowController::~WindowController() {
        delete m_window_impl;
        m_window_impl = nullptr;
    }

    std::unique_ptr<WindowController> WindowController::create() {
        auto result = std::make_unique<WindowController>();
        result->m_window_impl = new WindowImpl;
        return result;
    }

    bool WindowController::initialize() {
        m_width = 800;
        m_height = 600;
        m_title = "title";
        rg::EngineError::guarantee(m_window_impl != nullptr, "Must instantiate m_window_impl first");
        m_window_impl->window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_window_impl->window) {
            spdlog::error("Failed to create a GLFW window!");
            return false;
        }
        glfwMakeContextCurrent(m_window_impl->window);
        return true;
    }

    void WindowController::terminate() {
        glfwDestroyWindow(m_window_impl->window);
    }

    std::string_view WindowController::name() {
        return "WindowController";
    }

    std::unique_ptr<InputController> InputController::create() {
        return std::make_unique<InputController>();
    }

    bool InputController::initialize() {
        glfwSetKeyCallback(ControllerManager::get<WindowController>()->handle()->window, glfw_key_callback);
        initialize_key_maps();
        m_keys.resize(KEY_COUNT);
        for (int key = 0; key < m_keys.size(); ++key) {
            m_keys[key].m_key = static_cast<KeyId>(key);
        }
        return true;
    }

    Key &InputController::key(KeyId key) {
        EngineError::guarantee(key >= 0 && key < m_keys.size(), "KeyId out of bounds!");
        return m_keys[key];
    }

    const Key &InputController::key(KeyId key) const {
        EngineError::guarantee(key >= 0 && key < m_keys.size(), "KeyId out of bounds!");
        return m_keys[key];
    }

    void InputController::update_key(Key &key_data) {
        int glfw_key_code = g_engine_to_glfw_key[key_data.key()];
        auto window = ControllerManager::get<WindowController>()->handle()->window;
        int action = glfwGetKey(window, glfw_key_code);
        switch (key_data.state()) {
        case rg::Key::State::Released: {
            if (action == GLFW_PRESS) {
                key_data.m_state = Key::State::JustPressed;
            }
            break;
        }
        case rg::Key::State::JustReleased: {
            if (action == GLFW_PRESS) {
                key_data.m_state = rg::Key::State::JustPressed;
            } else if (action == GLFW_RELEASE) {
                key_data.m_state = Key::State::Released;
            }
            break;
        }
        case rg::Key::State::JustPressed: {
            if (action == GLFW_RELEASE) {
                key_data.m_state = Key::State::JustReleased;
            } else if (action == GLFW_PRESS) {
                key_data.m_state = Key::State::Pressed;
            }
            break;
        }
        case rg::Key::State::Pressed: {
            if (action == GLFW_RELEASE) {
                key_data.m_state = rg::Key::State::JustReleased;
            }
            break;
        }
        }
    }

    void InputController::update() {
        for (int i = 0; i < KEY_COUNT; ++i) {
            update_key(key(static_cast<KeyId>(i)));
        }
    }

    std::string_view InputController::name() {
        return "InputController";
    }

    class PlatformGLFW3 : public PlatformController {
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

    std::unique_ptr<PlatformController> PlatformController::create() {
        return std::make_unique<PlatformGLFW3>();
    }

    void initialize_key_maps() {
#include "glfw_key_mapping.h"
    }
}

static void glfw_key_callback(GLFWwindow *, int key, int, int action, int) {
    auto input = rg::ControllerManager::get<rg::InputController>();
    auto &key_data = input->key(rg::g_glfw_to_engine_key[key]);
    switch (key_data.state()) {
    case rg::Key::State::Released:
    case rg::Key::State::JustReleased: {
        if (action == GLFW_PRESS) {
            key_data.state() = rg::Key::State::JustPressed;
        }
        break;
    }
    case rg::Key::State::JustPressed:
    case rg::Key::State::Pressed: {
        if (action == GLFW_RELEASE) {
            key_data.state() = rg::Key::State::JustReleased;
        }
        break;
    }
    }
}

