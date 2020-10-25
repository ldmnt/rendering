#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;

    DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse);
};

struct PointLight {
    glm::vec3 position;
    
    glm::vec3 ambient;
    glm::vec3 diffuse;

    PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse);
};

#endif