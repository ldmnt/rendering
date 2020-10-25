#include "drawing.h"

namespace draw {
    void setDirectionalLight(Shader& shader, std::string name, DirectionalLight light) {
        shader.setVec3(name + ".direction", light.direction);
        shader.setVec3(name + ".ambient", light.ambient);
        shader.setVec3(name + ".diffuse", light.diffuse);
    }

    void setPointLight(Shader& shader, std::string name, PointLight light) {
        shader.setVec3(name + ".position", light.position);
        shader.setVec3(name + ".ambient", light.ambient);
        shader.setVec3(name + ".diffuse", light.diffuse);
    }
}