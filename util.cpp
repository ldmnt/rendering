#include "util.h"
#include <glm/fwd.hpp>
#include <sstream>
#include <fstream>
#include <glm/matrix.hpp>

using namespace std;

namespace util {
    string readFile(const char* filePath) {
        ifstream file;
        file.exceptions(ifstream::failbit | ifstream::badbit);
        try {
            file.open(filePath);
            stringstream stream;
            stream << file.rdbuf();
            file.close();
            return stream.str();
        }
        catch (ifstream::failure& e) {
            fprintf(stderr, "IO error: %s\n", e.what());
            return "";
        }
    }

    glm::vec4 cross(glm::vec4 a, glm::vec4 b, glm::vec4 c) {
        glm::vec4 d;
        d[0] = glm::determinant(glm::mat3(
            glm::vec3(a.y, a.z, a.w), glm::vec3(b.y, b.z, b.w), glm::vec3(c.y, c.z, c.w)
        ));
        d[1] = glm::determinant(glm::mat3(
            glm::vec3(a.x, a.z, a.w), glm::vec3(b.x, b.z, b.w), glm::vec3(c.x, c.z, c.w)
        ));
        d[2] = glm::determinant(glm::mat3(
            glm::vec3(a.x, a.y, a.w), glm::vec3(b.x, b.y, b.w), glm::vec3(c.x, c.y, c.w)
        ));
        d[3] = glm::determinant(glm::mat3(
            glm::vec3(a.x, a.y, a.z), glm::vec3(b.x, b.y, b.z), glm::vec3(c.x, c.y, c.z)
        ));
        return d;
    }
}
