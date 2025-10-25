#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

    void deinitialize();

    bool loop() override;

    //void poll_events() override;

    void draw_manor();

    void draw_street_lamp();

    void draw_tree();

    void draw_floor();

    void draw_grass();

    void draw_skybox();

    std::vector<float> generateSphereVertices(float radius, unsigned int sectors, unsigned int stacks);

    void draw_light_cube();

    void update_camera();

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }

private:
    unsigned int floorVAO, floorVBO;
    unsigned int grassVAO, grassVBO;
    unsigned int lightCubeVAO, lightCubeVBO;


};
} // app
#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
