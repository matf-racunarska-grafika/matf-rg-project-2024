#ifndef MYAPP_MAINCONTROLLER_HPP
#define MYAPP_MAINCONTROLLER_HPP

#include <string_view>
#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/core/Controller.hpp>
#include <GL/gl.h>

namespace engine::myapp {
struct ScheduledEvent {
    float triggerTime;// vreme kada se event aktivira
    std::string eventName;
};

class MainPlatformEventObserver final : public engine::platform::PlatformEventObserver {
public:
    void on_key(engine::platform::Key key) override;

    void on_mouse_move(engine::platform::MousePosition position) override;
};

class MainController final : public engine::core::Controller {
public:
    float pointLightIntensity = 5.0f;      // Intenzitet point light svetla
    glm::vec3 lightPos{-10.0f, 8.0f, 2.0f};// Pozicija point light svetla

    // -------Scheduled event--------------------------------------------------------
    // Queue svih zakazanih događaja
    std::vector<ScheduledEvent> eventQueue;

    // Flag i vreme za početnu akciju
    float currentTime = 0.0f;// u sekundama, broji vreme od starta
    bool actionTriggered = false;
    float actionTriggerTime = 0.0f;

    // Za treptanje svetla
    bool flickerActive = false;
    float flickerStartTime = 0.0f;

    // Spawn-ovani objekti: ime modela + pozicija, rotacija, skala
    std::vector<std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3> > spawnedObjects;

    void executeEvent(const std::string &eventName);

    // ---------------------------------------------------------------------------------------

    std::string_view name() const override { return "test::app::MainController"; }

private:
    int width, height;// prozor

    // MSAA off-screen anti-aliasing
    static constexpr unsigned int MSAA_SAMPLES = 4;// број узорака
    unsigned int msFBO = 0;                        // MSAA FBO
    unsigned int msColorRBO = 0;                   // MSAA color renderbuffer
    unsigned int msDepthRBO = 0;                   // MSAA depth-stencil renderbuffer

    // Point shadows
    static constexpr unsigned SHADOW_WIDTH = 2048;
    static constexpr unsigned SHADOW_HEIGHT = 2048;
    GLuint depthMapFBO = 0;
    GLuint depthCubemap = 0;
    float near_plane = 1.0f, far_plane = 25.0f;
    glm::mat4 shadowMatrices[6];

    // Point shadows funckije
    void renderSceneDepth(const resources::Shader *depthShader);

    void renderSceneLit(const resources::Shader *litShader);

    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void draw_mesh(auto model, auto shader, const glm::vec3 &position,
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

#endif // MYAPP_MAINCONTROLLER_HPP
