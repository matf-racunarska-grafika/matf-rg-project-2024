#ifndef MATF_RG_PROJECT_MAINCONTROLLER_H
#define MATF_RG_PROJECT_MAINCONTROLLER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/core/Controller.hpp"
#include <memory>

namespace app {
    class MainController : public engine::core::Controller {
    public:
        void initialize() override;
        bool loop() override;
        void begin_draw() override;
        void draw_moon();
        void draw_tulip();
        void draw();       // možeš da ostaviš samo jedan draw ako ne koristiš više faza
        void end_draw() override;
        void update() override;

    private:
        float m_rotation_angle = 0.0f; 
        glm::mat4 m_moon_transform;
        glm::mat4 m_tulip_transform;

            
    private:
        void update_camera();
    };
}

#endif 
