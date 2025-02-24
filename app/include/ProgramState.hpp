#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <glm/glm.hpp>

enum class Filter {
    NONE,
    NEGATIVE,
    GRAYSCALE,
    DEEPFRIED,
    OUTLINE,
    BLACKWHITE
};

class Settings {
public:
    static Settings& getInstance() {
        static Settings instance;
        return instance;
    }

    bool spectatorMode = false;
    glm::vec3 lightColor = glm::vec3(12.0f, 0.0f, 0.0f);
    unsigned int difficulty = 1;
    float skullSpeed = 160.0f;
    bool skullFacingPlayer = true;
    Filter filter = Filter::NONE;
    int health = 3;
    int maxHealth = 3;

private:
    Settings() {}
    ~Settings() {}
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
};

#endif // SETTINGS_HPP