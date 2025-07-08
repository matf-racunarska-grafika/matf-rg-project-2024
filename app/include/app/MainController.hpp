#ifndef MYAPP_MAINCONTROLLER_HPP
#define MYAPP_MAINCONTROLLER_HPP

#include <string_view>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/core/Controller.hpp>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <engine/graphics/MSAA.hpp>
#include <engine/graphics/Lighting.hpp>

namespace engine::myapp {

struct ScheduledEvent {
    float triggerTime;// vreme kada se event aktivira
    std::string eventName;
};

class MainController final : public engine::core::Controller {
public:
    // MSAA
    bool g_msaa_enabled = true;

    // Lighting
    float g_point_light_intensity = 7.0f;      // Intenzitet point light svetla
    glm::vec3 g_light_pos{-10.0f, 10.0f, 2.0f};// Pozicija point light svetla

    // -------Scheduled event--------------------------------------------------------
    // Queue svih zakazanih događaja
    std::vector<ScheduledEvent> g_event_queue;

    // Flag i vreme za početnu akciju
    float g_current_time = 0.0f;// u sekundama, broji vreme od starta
    bool g_action_triggered = false;
    float g_action_trigger_time = 0.0f;

    // Za treptanje svetla
    float g_flicker_duration = 2.0f;
    bool g_flicker_active = false;
    float g_flicker_start_time = 0.0f;

    float g_spawn_delay = 3.0f;

    // Spawn-ovani objekti: ime modela + pozicija, rotacija, skala
    struct SpawnedObject {
        std::string name;
        glm::vec3 position;
        glm::vec3 rotation;// Euler uglovi u radijanima
        glm::vec3 scale;
    };

    std::vector<SpawnedObject> g_spawned_objects;

    void execute_event(const std::string &eventName);

    // ---------------------------------------------------------------------------------------

    std::string_view name() const override { return "test::app::MainController"; }

private:
    // MSAA
    std::unique_ptr<engine::graphics::MSAA> m_msaa;

    // Lighting
    engine::graphics::lighting::LightingSystem m_lighting{2048, 2048};

    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void draw_mesh(auto model, auto shader,
                   const glm::vec3 &position,
                   const glm::vec3 &rotation,
                   const glm::vec3 &scale);

    void draw_light_source_mesh(const glm::vec3 &lightPos, float scale);

    void set_lights(auto shader);

    void draw_skybox();

    void update_camera();

    float m_backpack_scale{1.0f};
    bool m_draw_gui{false};
    bool m_cursor_enabled{true};
};

}// namespace engine::myapp

#endif
