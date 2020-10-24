#include "model.h"
#include <glm/gtc/constants.hpp>
#include <sstream>


Vertex::Vertex() {}

Vertex::Vertex(glm::vec3 position, glm::vec3 normal) : position(position), normal(normal) {}

Face::Face() {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Face> faces, bool supportFlatShading) : 
    vertices(vertices), faces(faces), supportFlatShading(supportFlatShading) {
    bufferData();
    if (supportFlatShading) {
        bufferFlatShadingData();
    }
}

Mesh::~Mesh() {
    unbufferData();
    if (supportFlatShading) {
        unbufferFlatShadingData();
    }
}

int Mesh::faceCount() {
    return faces.size();
}

void Mesh::bufferData() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(Face), faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Mesh::bufferFlatShadingData() {
    std::vector<Vertex> flatVertices(3 * faces.size());
    for (int i = 0; i < faces.size(); i++) {
        glm::vec3 normal = glm::cross(
            vertices[faces[i].indices[1]].position - vertices[faces[i].indices[0]].position,
            vertices[faces[i].indices[2]].position - vertices[faces[i].indices[0]].position
        );
        normal = glm::normalize(normal);
        for (int j = 0; j < 3; j++) {
            flatVertices[3 * i + j] = Vertex(vertices[faces[i].indices[j]].position, normal);
        }
    }
    
    glGenVertexArrays(1, &vaoFlat);
    glGenBuffers(1, &vboFlat);

    glBindVertexArray(vaoFlat);
    glBindBuffer(GL_ARRAY_BUFFER, vboFlat);
    glBufferData(GL_ARRAY_BUFFER, flatVertices.size() * sizeof(Vertex), flatVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Mesh::unbufferData() {
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::unbufferFlatShadingData() {
    glDeleteBuffers(1, &vaoFlat);
    glDeleteBuffers(1, &vboFlat);
}

void Mesh::draw(Shader &shader, bool flatShading) {
    shader.use();
    if (flatShading) {
        glBindVertexArray(vaoFlat);
        glDrawArrays(GL_TRIANGLES, 0, 3 * faces.size());
    }
    else {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, faces.size() * sizeof(Face), GL_UNSIGNED_INT, (GLvoid*)0);
    }
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
        printf("(%d, %d, %d)\n", faces[i].indices[0], faces[i].indices[1], faces[i].indices[2]);
    }
}

namespace mdl {
    Mesh generateCone(int sides, float scale, float aspectRatio) {
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
                scale * glm::vec3(0.0f, 0.0f, aspectRatio),
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

        return Mesh(vertices, faces, true);
    }
}