#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <glm/glm.hpp>

class Settings {
public:
    static Settings& getInstance() {
        static Settings instance;
        return instance;
    }

    glm::vec3 lightColor = glm::vec3(100.0f, 0.0f, 0.0f);
    unsigned int difficulty = 1;
    float skullSpeed = 160.0f;

private:
    Settings() {} // Private constructor
    ~Settings() {}
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
};

#endif // SETTINGS_HPP