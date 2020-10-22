#include "model.h"
#include <glm/gtc/constants.hpp>
#include <sstream>
#include <stdexcept>


Vertex::Vertex() : position(glm::vec3()), normal(glm::vec3()) {};

Vertex::Vertex(glm::vec3 position, glm::vec3 normal) :
    position(position), normal(normal) {}

Face::Face() {}

Face::Face(int i, int j, int k) {
    indices[0] = i;
    indices[1] = j;
    indices[2] = k;
}

unsigned int& Face::operator[](int i) {
    return indices[i];
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Face> faces) : 
    vertices(vertices), faces(faces) {}

std::vector<float> Mesh::renderingData() {
    std::vector<float> data(6 * vertices.size());
    glm::vec3 v;
    int k;
    for (int i = 0; i < vertices.size(); i++) {
        k = 6 * i;
        v = vertices[i].position;
        data[k] = v.x;
        data[k + 1] = v.y;
        data[k + 2] = v.z;
        k += 3;
        v = vertices[i].normal;
        data[k] = v.x;
        data[k + 1] = v.y;
        data[k + 2] = v.z;
    }
    return data;
}

std::vector<unsigned int> Mesh::renderingIndices() {
    std::vector<unsigned int> indices(3 * faces.size());
    int k;
    for (int i = 0; i < faces.size(); i++) {
        k = 3 * i;
        indices[k] = faces[i][0];
        indices[k + 1] = faces[i][1];
        indices[k + 2] = faces[i][2];
    }
    return indices;
}

void Mesh::print() {
    printf("vertices:\n");
    for (int i = 0; i < vertices.size(); i++) {
        printf("vertex (%.2f, %.2f, %.2f) - normal (%.2f, %.2f, %.2f)\n", 
            vertices[i].position[0], vertices[i].position[1], vertices[i].position[2],
            vertices[i].normal[0], vertices[i].normal[1], vertices[i].normal[2]
        );
    }
    printf("\nfaces\n");
    for (int i = 0; i < faces.size(); i++) {
        printf("(%d, %d, %d)\n", faces[i][0], faces[i][1], faces[i][2]);
    }
}

namespace mdl {
    Mesh generateCone(int sides, float scale) {
        std::vector<Vertex> vertices(2 * sides + 2);
        glm::vec3 dir;
        double angle;

        for (int i = 0; i < sides; i++) {
            angle = 2 * i * glm::pi<double>() / sides;
            dir = glm::vec3(cos(angle), sin(angle), 0.0f);
            vertices[i] = Vertex(
                scale * dir,
                dir
            );
            vertices[sides + i] = Vertex(
                scale * dir,
                glm::vec3(0.0f, 0.0f, - 1.0f)
            );
        }
        dir = glm::vec3(0.0f, 0.0f, 1.0f);
        vertices[2 * sides] = Vertex(
            scale * dir,
            dir
        );
        vertices[2 * sides + 1] = Vertex(
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, - 1.0f)
        );

        std::vector<Face> faces(2 * sides);
        for (int i = 0; i < sides; i++) {
            faces[i] = Face(i, (i + 1) % sides, 2 * sides);
            faces[sides + i] = Face(sides + (i + 1) % sides, sides + i, 2 * sides + 1);
        }

        return Mesh(vertices, faces);
    }
}