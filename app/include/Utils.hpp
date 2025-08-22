//
// Created by filip on 8/21/25.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <MainController.hpp>

#include <spdlog/spdlog.h>

#define VECTOR3_A(x,y,z) glm::vec3(x,y,z)
#define VECTOR3 glm::vec3
#define VECTOR4_A(x,y,z,w) glm::vec4(x,y,z,w)
#define VECTOR4 glm::vec4

struct dirLight{
        VECTOR3 direction;
        VECTOR3 diffuse;
        VECTOR3 ambient;
        VECTOR3 specular;
};

struct pointLight{
      VECTOR3 position;
      VECTOR3 diffuse;
      VECTOR3 ambient;
      VECTOR3 specular;
      VECTOR3 attenuation;
      float constantAttenuation;
      float linearAttenuation;
      float quadraticAttenuation;
};


struct spotLight{
    VECTOR3 position;
    VECTOR3 direction;
    VECTOR3 ambient;
    VECTOR3 specular;
    VECTOR3 diffuse;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

#endif //UTILS_HPP
