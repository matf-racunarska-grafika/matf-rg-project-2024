
#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <string>
#include <GLFW/glfw3.h>

namespace engine::platform {
    class Window final {
        friend class PlatformController;

    public:
        int height() const {
            return m_height;
        }

        int width() const {
            return m_width;
        }

        const std::string &title() const {
            return m_title;
        }

        GLFWwindow *handle_() const {
            return m_handle;
        }

    private:
        GLFWwindow *handle() const {
            return m_handle;
        }

        GLFWwindow *m_handle{};
        int m_width{};
        int m_height{};
        std::string m_title{};

        Window() = default;

        Window(GLFWwindow *handle, int width, int height, std::string title) : m_handle(handle)
                                                                           , m_width(width)
                                                                           , m_height(height)
                                                                           , m_title(std::move(title)) {
        }
    };
}
#endif //WINDOW_HPP
