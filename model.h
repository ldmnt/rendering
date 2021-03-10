#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <string>
#include <assimp/Importer.hpp>
#include "shader.h"


struct Vertex {
    glm::vec4 position;
    glm::vec3 normal;
    
    Vertex();
    Vertex(glm::vec4 position, glm::vec3 normal);
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

    unsigned int vao, vbo, ebo;
    void bufferData();
    void unbufferData();

public:
    Mesh(std::vector<Vertex> vertices, std::vector<Face> faces);
    Mesh(const Mesh&) = delete;
    Mesh &operator=(const Mesh&) = delete;
    Mesh(Mesh&& other);
    Mesh &operator=(Mesh&&);
    ~Mesh();
    void release();
    int faceCount();
    void draw(Shader &shader);
    void print();
};

class Model {
private:
    std::vector<std::unique_ptr<Mesh>> meshes;

public:
    Model(char* path);
    void draw(Shader& shader);
};
