#include <string>
#include <glm/glm.hpp>

namespace util {
    std::string readFile(const char* filePath);
    glm::vec4 cross(glm::vec4 a, glm::vec4 b, glm::vec4 c);
}
