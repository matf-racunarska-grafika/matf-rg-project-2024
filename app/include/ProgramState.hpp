#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <glm/glm.hpp>

class Settings {
public:
    static Settings &get_instance() {
        static Settings instance;
        return instance;
    }

    bool spectator_mode      = false;
    glm::vec3 light_color    = glm::vec3(12.0f, 0.0f, 0.0f);
    unsigned int difficulty  = 1;
    float skull_speed        = 160.0f;
    bool skull_facing_player = true;
    int health               = 3;
    int max_health           = 3;
    float teleport_cooldown  = 0.0f;

private:
    Settings() {
    }

    ~Settings() {
    }

    Settings(const Settings &) = delete;

    Settings &operator=(const Settings &) = delete;
};

#endif // SETTINGS_HPP
