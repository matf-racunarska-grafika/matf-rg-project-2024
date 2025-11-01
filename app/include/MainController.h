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

        // Parametri svetla – dostupni GUI-ju
        struct DirLightParams {
            glm::vec3 direction{ -0.2f, -1.0f, -0.3f };
            glm::vec3 ambient{ 0.30f };
            glm::vec3 diffuse{ 1.2f };
            glm::vec3 specular{ 2.0f };
        };
        struct PointLightParams {
            glm::vec3 position{ 12.0f, 14.0f, 10.0f };
            float constant{ 1.0f };
            float linear{ 0.09f };
            float quadratic{ 0.032f };
            glm::vec3 ambient{ 0.05f };
            glm::vec3 diffuse{ 0.4f };
            glm::vec3 specular{ 0.5f };
        };
        DirLightParams &dir_light() { return m_dir_light; }
        PointLightParams &point_light() { return m_point_light; }
        float &shininess() { return m_shininess; }

    private:
        float m_rotation_angle = 0.0f; 
        glm::mat4 m_moon_transform;
        glm::mat4 m_tulip_transform;

        DirLightParams m_dir_light{};
        PointLightParams m_point_light{};
        float m_shininess = 32.0f;

        // Event sekvenca (ACTION -> after t -> EVENT_A -> after t -> EVENT_B ...)
        bool  m_sequence_running = false;
        float m_sequence_time    = 0.0f;   // sekunde od ACTION
        bool  m_event_a_done     = false;  // promena boje svetla
        bool  m_event_b_done     = false;  // pomeraj meseca
        bool  m_event_c_done     = false;  // sakrij tulip
        bool  m_tulip_visible    = true;
        glm::vec3 m_moon_event_offset{0.0f};

            
    private:
        void update_camera();
    };
}

#endif 
