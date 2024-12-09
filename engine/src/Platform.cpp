//
// Created by spaske00 on 13.5.24.
//

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine/platform/PlatformController.hpp>
#include <engine/util/Utils.hpp>
#include <engine/controller/ControllerManager.hpp>
#include <spdlog/spdlog.h>
#include <utility>
#include <engine/graphics/OpenGL.hpp>
#include <engine/util/Configuration.hpp>

namespace engine::platform {
    static std::array<std::string_view, KEY_COUNT> g_engine_key_to_string;
    static std::array<int, KEY_COUNT> g_engine_to_glfw_key;
    static std::array<KeyId, GLFW_KEY_LAST + 1> g_glfw_key_to_engine;
    static MousePosition g_mouse_position;

    static void glfw_mouse_callback(GLFWwindow *window, double x, double y);

    static void glfw_scroll_callback(GLFWwindow *window, double x_offset, double y_offset);

    static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height);

    static int glfw_platform_action(GLFWwindow *window, int glfw_key_code);

    void initialize_key_maps();

    void PlatformController::initialize() {
        if (glfwPlatformSupported(GLFW_PLATFORM_X11)) {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
        } else if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
        } else if (glfwPlatformSupported(GLFW_PLATFORM_WIN32)) {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
        }
        bool glfw_initialized = glfwInit();
        RG_GUARANTEE(glfw_initialized, "GLFW platform failed to initialize_controllers.");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        util::Configuration::json &config = util::Configuration::config();
        int window_width = config["window"]["width"];
        int window_height = config["window"]["height"];
        std::string window_title = config["window"]["title"];
        GLFWwindow *handle = glfwCreateWindow(window_width, window_height, window_title.c_str(), nullptr, nullptr);
        RG_GUARANTEE(handle, "GLFW3 platform failed to create a Window.");
        m_window = Window(handle, window_width, window_height, window_title);

        glfwMakeContextCurrent(m_window.handle());
        glfwSetCursorPosCallback(m_window.handle(), glfw_mouse_callback);
        glfwSetScrollCallback(m_window.handle(), glfw_scroll_callback);
        glfwSetKeyCallback(m_window.handle(), glfw_key_callback);
        glfwSetFramebufferSizeCallback(m_window.handle(), glfw_framebuffer_size_callback);

        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);
        spdlog::info("Platform[GLFW {}.{}.{}]", major, minor, revision);
        initialize_key_maps();
        m_keys.resize(KEY_COUNT);
        for (int key = 0; key < m_keys.size(); ++key) {
            m_keys[key].m_key = static_cast<KeyId>(key);
        }

        register_platform_event_observer(std::make_unique<PlatformEventObserver>());
    }

    void PlatformController::terminate() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        m_platform_event_observer.reset(nullptr);
        glfwDestroyWindow(m_window.handle());
        glfwTerminate();
    }

    bool PlatformController::loop() {
        m_frame_time.previous = m_frame_time.current;
        m_frame_time.current  = glfwGetTime();
        m_frame_time.dt       = m_frame_time.current - m_frame_time.previous;

        return !glfwWindowShouldClose(m_window.handle());
    }

    void PlatformController::poll_events() {
        g_mouse_position.dx = g_mouse_position.dy = 0.0f;
        glfwPollEvents();
        for (int i = 0; i < KEY_COUNT; ++i) {
            update_key(key(static_cast<KeyId>(i)));
        }
    }

    void PlatformController::swap_buffers() {
        glfwSwapBuffers(m_window.handle());
    }

    int glfw_platform_action(GLFWwindow *window, int glfw_key_code) {
        if (glfw_key_code >= GLFW_MOUSE_BUTTON_1 && glfw_key_code <= GLFW_MOUSE_BUTTON_LAST) {
            return glfwGetMouseButton(window, glfw_key_code);
        }
        return glfwGetKey(window, glfw_key_code);
    }

    void PlatformController::update_key(Key &key_data) const {
        int engine_key_code = key_data.id();
        int glfw_key_code   = g_engine_to_glfw_key.at(engine_key_code);
        int action          = glfw_platform_action(m_window.handle(), glfw_key_code);
        switch (key_data.state()) {
        case Key::State::Released: {
            if (action == GLFW_PRESS) {
                key_data.m_state = Key::State::JustPressed;
            }
            break;
        }
        case Key::State::JustReleased: {
            if (action == GLFW_PRESS) {
                key_data.m_state = Key::State::JustPressed;
            } else if (action == GLFW_RELEASE) {
                key_data.m_state = Key::State::Released;
            }
            break;
        }
        case Key::State::JustPressed: {
            if (action == GLFW_RELEASE) {
                key_data.m_state = Key::State::JustReleased;
            } else if (action == GLFW_PRESS) {
                key_data.m_state = Key::State::Pressed;
            }
            break;
        }
        case Key::State::Pressed: {
            if (action == GLFW_RELEASE) {
                key_data.m_state = Key::State::JustReleased;
            }
            break;
        }
        }
    }

    std::string_view Key::key_name() {
        return g_engine_key_to_string[m_key];
    }

    std::string_view to_string(Key::State state) {
        switch (state) {
        case Key::State::Released: return "Released";
        case Key::State::JustPressed: return "JustPressed";
        case Key::State::Pressed: return "Pressed";
        case Key::State::JustReleased: return "JustReleased";
        default: return "UNIMPLEMENTED";
        }
    }

    Key &PlatformController::key(KeyId key) {
        RG_GUARANTEE(key >= 0 && key < m_keys.size(), "KeyId out of bounds!");
        return m_keys[key];
    }

    void PlatformEventObserver::on_mouse(MousePosition position) {
    }

    void PlatformEventObserver::on_keyboard(Key key) {
    }

    void PlatformEventObserver::on_window_resize(int width, int height) {
    }

    const Key &PlatformController::key(KeyId key) const {
        RG_GUARANTEE(key >= 0 && key < m_keys.size(), "KeyId out of bounds!");
        return m_keys[key];
    }

    const MousePosition &PlatformController::mouse() const {
        return g_mouse_position;
    }

    std::string_view PlatformController::name() const {
        return "PlatformGLFW3Controller";
    }

    void PlatformController::register_platform_event_observer(std::unique_ptr<PlatformEventObserver> observer) {
        m_platform_event_observer = std::move(observer);
    }

    void PlatformController::_platform_on_mouse(double x, double y) {
        double last_x       = g_mouse_position.x;
        double last_y       = g_mouse_position.y;
        g_mouse_position.dx = x - last_x;
        g_mouse_position.dy = last_y - y; // because in glfw the top left corner is the (0,0)
        g_mouse_position.x  = x;
        g_mouse_position.y  = y;
        m_platform_event_observer->on_mouse(g_mouse_position);
    }

    void PlatformController::_platform_on_keyboard(int key_code, int action) {
        const Key result = key(g_glfw_key_to_engine[key_code]);
        m_platform_event_observer->on_keyboard(result);
    }

    void PlatformController::_platform_on_scroll(double x, double y) {
        g_mouse_position.scroll = y;
        m_platform_event_observer->on_mouse(g_mouse_position);
    }

    void PlatformController::_platform_on_framebuffer_resize(int width, int height) {
        m_window.m_width  = width;
        m_window.m_height = height;
        m_platform_event_observer->on_window_resize(width, height);
    }

    void PlatformController::set_enable_cursor(bool enabled) {
        if (enabled) {
            glfwSetInputMode(m_window.handle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(m_window.handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    void initialize_key_maps() {
#include "glfw_key_mapping.include"
#include "engine_key_to_string.include"
    }

    static void glfw_mouse_callback(GLFWwindow *window, double x, double y) {
        controller::get<PlatformController>()->_platform_on_mouse(x, y);
    }

    static void glfw_scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
        g_mouse_position.scroll = y_offset;
        controller::get<PlatformController>()->_platform_on_scroll(x_offset, y_offset);
    }

    static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        controller::get<PlatformController>()->_platform_on_keyboard(key, action);
    }

    static void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
        controller::get<PlatformController>()->_platform_on_framebuffer_resize(width, height);
    }

} // namespace engine
