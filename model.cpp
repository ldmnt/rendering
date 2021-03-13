#include "model.h"
#include <glm/gtc/constants.hpp>
#include <sstream>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <functional>

Vertex::Vertex() {}

Vertex::Vertex(glm::vec4 position, glm::vec3 normal) : position(position), normal(normal) {}

Face::Face() {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Face> faces) : vertices(vertices), faces(faces) {
    bufferData();
}

Mesh::Mesh(Mesh&& other) {
    vertices = other.vertices;
    faces = other.faces;
    vao = other.vao;
    vbo = other.vbo;
    ebo = other.ebo;

    other.vao = 0;
}

Mesh& Mesh::operator=(Mesh&& other) {
    if (this != &other) {
        release();
        vertices = other.vertices;
        faces = other.faces;
        vao = other.vao;
        vbo = other.vbo;
        ebo = other.ebo;

        other.vao = 0;
    }
    return *this;
}

void Mesh::release() {
    if (vao == 0)
        return;
    unbufferData();
}

Mesh::~Mesh() {
    release();
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

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(4 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Mesh::unbufferData() {
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::draw(Shader &shader) {
    shader.use();
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, faces.size() * sizeof(Face), GL_UNSIGNED_INT, (GLvoid*)0);
}

void Mesh::print() {
    printf("vertices:\n");
    for (int i = 0; i < vertices.size(); i++) {
        printf("%d - vertex (%.2f, %.2f, %.2f) - normal (%.2f, %.2f, %.2f)\n", 
            i,
            vertices[i].position[0], vertices[i].position[1], vertices[i].position[2],
            vertices[i].normal[0], vertices[i].normal[1], vertices[i].normal[2]
        );
    }
    printf("\nfaces\n");
    for (int i = 0; i < faces.size(); i++) {
        printf("(%d, %d, %d)\n", faces[i].indices[0], faces[i].indices[1], faces[i].indices[2]);
    }
}

void Model::print() {
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i].get()->print();
    }
}

Model::Model(char* path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("Assimp error: %s\n", importer.GetErrorString());
        return;
    }

    auto processMesh = [&](aiMesh* mesh) {
        std::vector<Vertex> vertices;
        std::vector<Face> faces;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex v(
                glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f),
                glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
            );
            vertices.push_back(v);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            Face f(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
            faces.push_back(f);
        }
        
        Mesh *m = new Mesh(vertices, faces);
        return std::unique_ptr<Mesh>(m);
    };

    std::function<void(aiNode*)> processNode = [&](aiNode* node) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i]);
        }
    };

    processNode(scene->mRootNode);
}

Model::Model(std::unique_ptr<Mesh> &mesh) {
    meshes.push_back(std::move(mesh));
}

void Model::draw(Shader& shader) {
    for (auto it = meshes.begin(); it != meshes.end(); it++) {
        (*it)->draw(shader);
    }
}

namespace mdl {
    Model generatePolyhedron(std::vector<glm::vec3> vertices, std::vector<Face> faces) {
        auto mesh_vertices = std::vector<Vertex>();
        auto mesh_faces = std::vector<Face>();
        for (int k = 0; k < faces.size(); ++k) {
            auto v1 = vertices[faces[k].indices[0]];
            auto v2 = vertices[faces[k].indices[1]];
            auto v3 = vertices[faces[k].indices[2]];
            auto normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
            mesh_vertices.push_back(Vertex(glm::vec4(v1, 1.0f), normal));
            mesh_vertices.push_back(Vertex(glm::vec4(v2, 1.0f), normal));
            mesh_vertices.push_back(Vertex(glm::vec4(v3, 1.0f), normal));
            mesh_faces.push_back(Face(3 * k, 3 * k + 1, 3 * k + 2));
        }
        Mesh *m = new Mesh(mesh_vertices, mesh_faces);
        auto res = std::unique_ptr<Mesh>(m);
        return Model(res);
    }

    Model euclideanPyramid() {
        auto vertices = std::vector<glm::vec3> {
            glm::vec3(0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, 0.5f, -0.5f),
            glm::vec3(-0.5f, 0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(0.0f, 0.0f, 0.5f)
        };
        auto faces = std::vector<Face> {
            Face(1, 0, 2),
            Face(0, 3, 2),
            Face(0, 1, 4),
            Face(1, 2, 4),
            Face(2, 3, 4),
            Face(3, 0, 4)
        };
        return generatePolyhedron(vertices, faces);
    }

    Model euclideanCube() {
        auto vertices = std::vector<glm::vec3> {
            glm::vec3(0.5, 0.5, 0.5),
            glm::vec3(-0.5, 0.5, 0.5),
            glm::vec3(-0.5, -0.5, 0.5),
            glm::vec3(0.5, -0.5, 0.5),
            glm::vec3(0.5, -0.5, -0.5),
            glm::vec3(0.5, 0.5, -0.5),
            glm::vec3(-0.5, 0.5, -0.5),
            glm::vec3(-0.5, -0.5, -0.5)
        };
        auto faces = std::vector<Face> {
            Face(0, 1, 2), Face(0, 2, 3),
            Face(0, 5, 6), Face(0, 6, 1),
            Face(1, 6, 7), Face(1, 7, 2),
            Face(3, 7, 4), Face(3, 2, 7),
            Face(7, 5, 4), Face(7, 6, 5),
            Face(3, 4, 5), Face(3, 5, 0),
        };
        return generatePolyhedron(vertices, faces);
    }
}
