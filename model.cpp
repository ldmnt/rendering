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

int Mesh::faceCount() {
    return faces.size();
}

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

std::vector<float> Mesh::flatRenderingData() {
    std::vector<float> data(18 * faces.size());
    glm::vec3 n, v;
    for (int i = 0; i < faces.size(); i++) {
        n = glm::cross(
            vertices[faces[i][1]].position - vertices[faces[i][0]].position,
            vertices[faces[i][2]].position - vertices[faces[i][0]].position
        );
        n = glm::normalize(n);
        for (int j = 0; j < 3; j++) {
            v = vertices[faces[i][j]].position;
            data[18 * i + 6 * j] = v.x;
            data[18 * i + 6 * j + 1] = v.y;
            data[18 * i + 6 * j + 2] = v.z;
            data[18 * i + 6 * j + 3] = n.x;
            data[18 * i + 6 * j + 4] = n.y;
            data[18 * i + 6 * j + 5] = n.z;
        }
    }
    return data;
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
        std::vector<Vertex> vertices(3 * sides + 1);
        glm::vec3 dir;
        double angle;

        for (int i = 0; i < sides; i++) {
            angle = 2 * i * glm::pi<double>() / sides;
            dir = glm::vec3(cos(angle), sin(angle), 0.0f);
            vertices[2 * i] = Vertex(
                scale * dir,
                dir
            );

            angle = 2 * (i + 0.5) * glm::pi<double>() / sides;
            vertices[2 * i + 1] = Vertex(
                scale * glm::vec3(0.0f, 0.0f, 1.0f),
                glm::normalize(glm::vec3(cos(angle), sin(angle), 1))
            );

            vertices[2 * sides + i] = Vertex(
                scale * dir,
                glm::vec3(0.0f, 0.0f, - 1.0f)
            );
        }

        vertices[3 * sides] = Vertex(
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, - 1.0f)
        );

        std::vector<Face> faces(2 * sides);
        for (int i = 0; i < sides; i++) {
            faces[i] = Face(2 * i, 2 * (i + 1) % (2 * sides), 2 * i + 1);
            faces[sides + i] = Face(2 * sides + (i + 1) % sides, 2 * sides + i, 3 * sides);
        }

        return Mesh(vertices, faces);
    }
}