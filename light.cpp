#include "light.h"

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse) :
    direction(glm::normalize(direction)), ambient(ambient), diffuse(diffuse) {}

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse) :
    position(position), ambient(ambient), diffuse(diffuse) {}