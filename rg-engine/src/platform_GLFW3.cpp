//
// Created by spaske00 on 13.5.24..
//

#include "engine/platform.hpp"
#include "engine/utils.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <utility>

namespace rg {

    static std::array<int, KEY_COUNT> g_engine_to_glfw_key;
    static MousePosition g_mouse_position;

    static void glfw_mouse_callback(GLFWwindow *window, double x, double y);

    void initialize_key_maps();

    struct WindowImpl {
        GLFWwindow *handle;
        int width;
        int height;
        std::string title;

        WindowImpl(GLFWwindow *handle, int width, int height, std::string title) : handle(handle), width(width),
                                                                                          height(height), title(std::move(title)) {
        }
    };

    void PlatformController::initialize() {
        bool glfw_initialized = glfwInit();
        rg::guarantee(glfw_initialized, "GLFW platform failed to initialize.");
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        const Configuration::json &config = ControllerManager::get<Configuration>()->config();
        int window_width = config["window"]["width"];
        int window_height = config["window"]["height"];
        std::string window_title = config["window"]["title"];
        GLFWwindow *handle = glfwCreateWindow(window_width, window_height,
                                              window_title.c_str(), nullptr, nullptr);
        rg::guarantee(handle, "GLFW3 platform failed to create a Window.");
        m_window = new WindowImpl(handle, window_width, window_height, window_title);
        rg::guarantee(m_window != nullptr, "Must instantiate m_window_impl first");

        glfwMakeContextCurrent(m_window->handle);
        glfwSetCursorPosCallback(m_window->handle, glfw_mouse_callback);
        rg::guarantee(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress), "GLAD failed to init!");

        initialize_key_maps();
        m_keys.resize(KEY_COUNT);
        for (int key = 0; key < m_keys.size(); ++key) {
            m_keys[key].m_key = static_cast<KeyId>(key);
        }
    }

    void PlatformController::terminate() {
        glfwDestroyWindow(m_window->handle);
        delete m_window;
        m_window = nullptr;
        glfwTerminate();
    }


    bool PlatformController::loop() {
        return true;
    }

    void PlatformController::poll_events() {
        glfwPollEvents();
    }

    void PlatformController::update() {
        m_mouse = g_mouse_position;
        for (int i = 0; i < KEY_COUNT; ++i) {
            update_key(key(static_cast<KeyId>(i)));
        }
    }

    void PlatformController::update_key(Key &key_data) {
        int engine_key_code = key_data.key();
        int glfw_key_code = g_engine_to_glfw_key.at(engine_key_code);
        auto window = m_window->handle;
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


    std::string_view Key::to_string() const {
        switch (m_state) {
        case Key::State::Released: return "Released";
        case Key::State::JustPressed: return "JustPressed";
        case Key::State::Pressed: return "Pressed";
        case Key::State::JustReleased: return "JustReleased";
        default: return "UNIMPLEMENTED";
        }
    }

    Key &PlatformController::key(KeyId key) {
        rg::guarantee(key >= 0 && key < m_keys.size(), "KeyId out of bounds!");
        return m_keys[key];
    }

    const Key &PlatformController::key(KeyId key) const {
        rg::guarantee(key >= 0 && key < m_keys.size(), "KeyId out of bounds!");
        return m_keys[key];
    }


    std::unique_ptr<PlatformController> PlatformController::create() {
        return std::make_unique<PlatformController>();
    }

    const MousePosition &PlatformController::mouse() const {
        return m_mouse;
    }

    std::string_view PlatformController::name() const {
        return "PlatformGLFW3Controller";
    }

    int PlatformController::window_width() const {
        return m_window->width;
    }

    int PlatformController::window_height() const {
        return m_window->height;
    }

    const std::string &PlatformController::window_title() const {
        return m_window->title;
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
