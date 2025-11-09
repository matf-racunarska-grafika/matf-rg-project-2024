//
// Created by matija on 10/12/25.
//

#include <engine/resources/Light.hpp>
#include <algorithm>
#include <engine/graphics/BloomFilter.hpp>
#include <engine/graphics/BloomFilter.hpp>

namespace engine::graphics {


Light::Light(LightType type,glm::vec3 position,glm::vec3 color,glm::vec3 direction)
    :m_position(position), m_direction(direction), m_color(color){
    m_type = type;
}

Light::Light(LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 color,
           float ambient_factor, float diffuse_factor, float specular_factor,
           float cut_off, float outer_cut_off,
           float constant, float linear, float quadratic)
    :m_position(position), m_direction(direction), m_color(color),
     m_cutoff(cut_off), m_outer_cutoff(outer_cut_off),
     m_constant(constant), m_linear(linear), m_quadratic(quadratic),
     m_diffuse(diffuse_factor),m_ambient(ambient_factor),m_specular(specular_factor) {
    m_type = type;
}

LightData Light::default_light() {
    return LightData {.constant = 1.f};
}

void Light::change_brightness(float alpha) {
    m_intensity = std::clamp(m_intensity * alpha, 0.0f, 1.0f);
}

void Light::set_brightness(float brightness) {
    m_intensity = brightness;
}

void Light::set_color(const glm::vec3 &color) {
    m_color = color;
}

void Light::set_direction(const glm::vec3 &direction) {
    m_direction = direction;
}

glm::vec3 Light::get_direction() const {
    return  m_direction;
}

void Light::set_attenuation(float constant, float linear, float quadratic) {
    m_constant = constant;
    m_linear = linear;
    m_quadratic = quadratic;

}

void Light::set_cutoff(float cutoff_deg, float outer_cutoff_deg) {
    m_cutoff = cutoff_deg;
    m_outer_cutoff = outer_cutoff_deg;
}

bool Light::is_spotlight() { return m_type == LightType::Spot; }

glm::vec3 Light::get_position() {
    return m_position;
}

void Light::set_position(glm::vec3 new_position) {
    m_position = new_position;
}

void Light::move_position(glm::vec3 delta) {
    m_position += delta;

}

LightType Light::light_type() const {
    return m_type;
}

LightData Light::light_data() const {
    return LightData {
        .position = m_position,
        .direction = m_direction,
        .cutoff = glm::cos(glm::radians( m_cutoff)),
        .outercutoff = glm::cos(glm::radians(m_outer_cutoff)),
        .constant = m_constant,
        .linear = m_linear,
        .quadratic = m_quadratic,
        .ambient = m_color * m_ambient * m_intensity,
        .diffuse = m_color * m_diffuse * m_intensity,
        .specular = m_color * m_specular * m_intensity
    };
}

std::string Light::uniform_name_convention(LightType type)  {
    switch (type) {
        case LightType::Directional: return "light_directional";
        case LightType::Point: return "light_point";
        case LightType::Spot: return "light_spot";
        default: RG_SHOULD_NOT_REACH_HERE("Unhandled slight type");
    }
}

}
