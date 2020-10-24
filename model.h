#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <string>
#include <assimp/Importer.hpp>
#include "shader.h"


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    
    Vertex();
    Vertex(glm::vec3 position, glm::vec3 normal);
};

struct Face {
    unsigned int indices[3];

    Face();
    Face(unsigned int i, unsigned int j, unsigned int k) {
        indices[0] = i;
        indices[1] = j;
        indices[2] = k;
    }
};

class Mesh {
private:
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    bool supportFlatShading;

    unsigned int vao, vbo, ebo;
    unsigned int vaoFlat, vboFlat;
    void bufferData();
    void bufferFlatShadingData();
    void unbufferData();
    void unbufferFlatShadingData();

public:
    Mesh(std::vector<Vertex> vertices, std::vector<Face> faces, bool supportFlatShading);
    Mesh(const Mesh&) = delete;
    Mesh &operator=(const Mesh&) = delete;
    Mesh(Mesh&& other);
    Mesh &operator=(Mesh&&);
    ~Mesh();
    void release();
    int faceCount();
    void draw(Shader &shader, bool flatShading);
    void print();
};

class Model {
private:
    std::vector<std::unique_ptr<Mesh>> meshes;

public:
    Model(char* path, bool supportFlatShading = false);
    void draw(Shader& shader, bool flatShading = false);
};

namespace mdl {
    Mesh generateCone(int sides, float scale, float aspectRatio = 1.0f);
}
