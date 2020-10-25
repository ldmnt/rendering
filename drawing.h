#include "light.h"
#include "shader.h"

namespace draw {
    void setDirectionalLight(Shader& shader, std::string name, DirectionalLight light);
    void setPointLight(Shader& shader, std::string name, PointLight light);
}