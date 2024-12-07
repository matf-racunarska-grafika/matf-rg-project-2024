//
// Created by spaske00 on 13.5.24.
//

#ifndef MATF_RG_PROJECT_PLATFORM_H
#define MATF_RG_PROJECT_PLATFORM_H

#include <rg/controller/Controller.hpp>
#include <memory>
#include <vector>
#include <rg/platform/Input.hpp>
#include <rg/platform/Window.hpp>
#include <rg/platform/PlatformEventObserver.hpp>
#include <rg/controller/ControllerManager.hpp>

struct GLFWwindow;

namespace rg::platform {
    struct FrameTime {
        float dt;
        float previous;
        float current;
    };

    class PlatformController final : public controller::Controller {
        friend class ControllerManager;

    public:
        const Key &key(KeyId key) const;

        Key &key(KeyId key);

        const MousePosition &mouse() const;

        std::string_view name() const override;

        const Window *window() const {
            return &m_window;
        }

        void register_platform_event_observer(std::unique_ptr<PlatformEventObserver> observer);

        FrameTime frame_time() const {
            return m_frame_time;
        }

        float dt() const {
            return m_frame_time.dt;
        }

        void _platform_on_mouse(double x, double y);

        void _platform_on_keyboard(int key, int action);

        void _platform_on_scroll(double x, double y);

        void _platform_on_framebuffer_resize(int width, int height);

        void set_enable_cursor(bool enabled);

        void swap_buffers();

    private:
        void initialize() override;

        void terminate() override;

        bool loop() override;

        void update_mouse();

        void poll_events() override;

        FrameTime m_frame_time;
        Window m_window;
        std::vector<Key> m_keys;
        std::unique_ptr<PlatformEventObserver> m_platform_event_observer;

        void update_key(Key &key_data) const;
    };
} // namespace rg

#endif//MATF_RG_PROJECT_PLATFORM_H
