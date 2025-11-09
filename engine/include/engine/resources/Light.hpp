#ifndef MATF_RG_PROJECT_LIGHT_HPP
#define MATF_RG_PROJECT_LIGHT_HPP

#include <engine/util/Errors.hpp>
#include <glm/vec3.hpp>
#include <string_view>

namespace engine::graphics {
enum class LightType {
    Point,
    Spot,
    Directional,
    None,
};

struct LightData {
    glm::vec3 position;
    glm::vec3 direction;

    float cutoff;
    float outercutoff;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class Light {

public:

    Light(LightType type, glm::vec3 position, glm::vec3 color, glm::vec3 direction={0.0f,-1.0f,0.0f});;

    Light(LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 color,
              float ambient_factor, float diffuse_factor, float specular_factor,
              float cut_off, float outer_cut_off,
              float constant, float linear, float quadratic);

    static LightData default_light();
    void change_brightness(float alpha);
    void set_brightness(float brightness);
    void set_color(const glm::vec3 &color);
    void set_direction(const glm::vec3 &direction);
    glm::vec3 get_direction() const;
    void set_attenuation(float constant, float linear, float quadratic);
    void set_cutoff(float cutoff_deg, float outer_cutoff_deg);

    bool is_spotlight();

    glm::vec3 get_position();
    void set_position(glm::vec3 new_position);
    void move_position(glm::vec3 delta);

    LightType light_type() const;

    LightData light_data() const;

    static std::string uniform_name_convention(LightType type);


protected:
    LightType m_type = LightType::Point;
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_direction = {0.0f, -1.0f, 0.0f};
    glm::vec3 m_color = {1.0f, 1.0f, 1.0f};

    float m_cutoff = 45;
    float m_outer_cutoff = 50;

    float m_constant = 1.0f;
    float m_linear = 0.12f;
    float m_quadratic = 0.082f;

    float m_intensity = 1.0f;
    float m_ambient = 0.02f;
    float m_diffuse = 0.8f;
    float m_specular = 1.0f;

};


}


#endif //MATF_RG_PROJECT_LIGHT_HPP