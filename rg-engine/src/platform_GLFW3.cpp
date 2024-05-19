//
// Created by spaske00 on 13.5.24..
//

#include "engine/platform.hpp"
#include "engine/utils.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace rg {

    static std::array<int, KEY_COUNT> g_engine_to_glfw_key;
    static std::array<KeyId, KEY_COUNT> g_glfw_to_engine_key;
    static MousePosition g_mouse_position;

    static void glfw_mouse_callback(GLFWwindow *window, double x, double y);

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

    void WindowController::initialize() {
        m_width = 800;
        m_height = 600;
        m_title = "title";
        rg::guarantee(m_window_impl != nullptr, "Must instantiate m_window_impl first");
        m_window_impl->window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        rg::guarantee(m_window_impl->window, "GLFW3 platform failed to create a Window.");
        glfwMakeContextCurrent(m_window_impl->window);
        glfwSetCursorPosCallback(m_window_impl->window, glfw_mouse_callback);
    }

    void WindowController::terminate() {
        glfwDestroyWindow(m_window_impl->window);
    }

    std::unique_ptr<InputController> InputController::create() {
        return std::make_unique<InputController>();
    }

    void InputController::initialize() {
        initialize_key_maps();
        m_keys.resize(KEY_COUNT);
        for (int key = 0; key < m_keys.size(); ++key) {
            m_keys[key].m_key = static_cast<KeyId>(key);
        }
    }

    Key &InputController::key(KeyId key) {
        guarantee(key >= 0 && key < m_keys.size(), "KeyId out of bounds!");
        return m_keys[key];
    }

    const Key &InputController::key(KeyId key) const {
        guarantee(key >= 0 && key < m_keys.size(), "KeyId out of bounds!");
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

    void InputController::update_mouse() {
        m_mouse = g_mouse_position;
    }

    class PlatformGLFW3 : public PlatformController {
    public:
        void initialize() override {
            bool glfw_initialized = glfwInit();
            rg::guarantee(glfw_initialized, "GLFW platform failed to initialize.");
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        }

        void terminate() override {
            glfwTerminate();
        }

        void poll_events() override {
            glfwPollEvents();
        }

        std::string_view name() const override {
            return "PlatformGLFW3";
        }
    };

    std::unique_ptr<PlatformController> PlatformController::create() {
        return std::make_unique<PlatformGLFW3>();
    }

    void initialize_key_maps() {
#include "glfw_key_mapping.h"
    }

    static void glfw_mouse_callback(GLFWwindow *window, double x, double y) {
        double last_x = g_mouse_position.x;
        double last_y = g_mouse_position.y;
        g_mouse_position.dx = x - last_x;
        g_mouse_position.dy = last_y - y;// because in glfw the top left corner is the (0,0)
        g_mouse_position.x = x;
        g_mouse_position.y = y;
    }

}// namespace rg
