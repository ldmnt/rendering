#include <vector>
#include <glm/glm.hpp>
#include <string>


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;

    Vertex();
    Vertex(glm::vec3 position, glm::vec3 normal);
};

struct Face {
    unsigned int indices[3];

    Face();
    Face(int i, int j, int k);
    unsigned int& operator[](int i);
};

class Mesh {
private:
    std::vector<Vertex> vertices;
    std::vector<Face> faces;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<Face> faces);
    std::vector<float> renderingData();
    std::vector<unsigned int> renderingIndices();
    void print();
};

namespace mdl {
    Mesh generateCone(int sides, float scale);
}
