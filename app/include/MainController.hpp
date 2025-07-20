#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/graphics/BloomEffect.hpp>
#include <glm/glm.hpp>

namespace app {
class MainController  : public engine::core::Controller
{
    void initialize() override;

    bool loop() override;
    void draw_temple();
    void update() override;
    void update_camera();
    void begin_draw() override;
    void draw_petal();
    void draw_ground();
    void draw_tree();
    void draw() override;
    void draw_skybox();
    void draw_lamp();
    void end_draw() override;

    //za eventove
    std::vector<glm::mat4> petalMatrices;
    std::vector<glm::mat4> originalPetalMatrices;
    bool initialized = false;

    bool waitingForPetalStart = false;
    float timeSinceAction = 0.0f;

    bool animatePetals = false;
    float petalAnimationTime = 0.0f;

    bool eventB_triggered = false;
    float timeSincePetalStart = 0.0f;

    float eventB_duration = 0.0f;
    bool restoreLighting = false;

    //za fejd svetla u event b
    glm::vec3 currentAmbient  = glm::vec3(0.2f);
    glm::vec3 currentDiffuse  = glm::vec3(0.5f);
    glm::vec3 currentSpecular = glm::vec3(0.8f);

    glm::vec3 targetAmbient = currentAmbient;
    glm::vec3 targetDiffuse = currentDiffuse;
    glm::vec3 targetSpecular = currentSpecular;

    //bloom
    engine::graphics::BloomEffect bloom;

public:
    std::string_view name() const override { return "app::MainController"; }
    void on_window_resize(int width, int height);

};
}



#endif //MAINCONTROLLER_HPP