#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "util.h"
#include "shader.h"
#include "model.h"

const int CONE_RESOLUTION = 128;

const float SCREEN_WIDTH = 640.0f;
const float SCREEN_HEIGHT = 480.0f;

const float CAMERA_SPEED = 2.5f;
const float CAMERA_FOV = 45.0f;
const float CAMERA_NEAR_PLANE = 0.02f;
const float CAMERA_FAR_PLANE = 1000.0f;
const glm::vec3 CAMERA_INITIAL_POS = glm::vec3(2.5f, 1.8f, 1.5f);
const glm::vec3 CAMERA_INITIAL_AIM = glm::normalize(glm::vec3(0.0f) - CAMERA_INITIAL_POS);

const float MOUSE_SENSITIVITY = 0.3f;

enum class ShadingMode { FLAT, GOURAUD, PHONG };
ShadingMode shadingMode = ShadingMode::PHONG;

bool firstMouse = true;
double lastMouseX, lastMouseY;

bool fixedCamera = true;
Camera camera(CAMERA_INITIAL_AIM, CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE, SCREEN_WIDTH / SCREEN_HEIGHT, CAMERA_FOV, CAMERA_INITIAL_POS);

glm::vec3 sunDir = glm::vec3(-1.5f, -1, -3);
float ambientLight = 0.2f;

GLuint vaoFlat, vboFlat, vao, vbo, ebo;
int nIndices, nVerticesFlat;
float lastFrame, deltaTime;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        if (!fixedCamera) {
            camera.lookAt(glm::vec3(0.0f));
        }
        fixedCamera = !fixedCamera;
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        shadingMode = ShadingMode::FLAT;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        shadingMode = ShadingMode::GOURAUD;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        shadingMode = ShadingMode::PHONG;
    }
}

static void mouse_callback(GLFWwindow* window, double xPosition, double yPosition) {
    if (firstMouse)
    {
        lastMouseX = xPosition;
        lastMouseY = yPosition;
        firstMouse = false;
    }

    float xOffset = (lastMouseX - xPosition) * MOUSE_SENSITIVITY;
    float yOffset = (lastMouseY - yPosition) * MOUSE_SENSITIVITY;

    if (fixedCamera) {
        camera.rotateAroundOrigin(xOffset, yOffset);
    }
    else {
        camera.turn(xOffset, yOffset);
    }
    
    lastMouseX = xPosition;
    lastMouseY = yPosition;
}

static void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera.moveForward(CAMERA_SPEED * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera.moveForward(-CAMERA_SPEED * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.moveRight(CAMERA_SPEED * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.moveRight(-CAMERA_SPEED * deltaTime);
    }
}

static void createGeometry() {
    Mesh mesh = mdl::generateCone(CONE_RESOLUTION, 1);

    std::vector<float> vertexData = mesh.renderingData();
    std::vector<unsigned int> indices = mesh.renderingIndices();
    nIndices = indices.size();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::vector<float> vertexDataFlat = mesh.flatRenderingData();
    nVerticesFlat = 3 * mesh.faceCount();
    glGenVertexArrays(1, &vaoFlat);
    glGenBuffers(1, &vboFlat);

    glBindVertexArray(vaoFlat);
    glBindBuffer(GL_ARRAY_BUFFER, vboFlat);
    glBufferData(GL_ARRAY_BUFFER, vertexDataFlat.size() * sizeof(float), vertexDataFlat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

static void deleteGeometry() {
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vaoFlat);
    glDeleteBuffers(1, &vboFlat);
}

static void render(Shader &gouraudShader, Shader &phongShader) {
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 transform = glm::mat4(1.0f);

    if (shadingMode == ShadingMode::PHONG) {
        phongShader.setMat4("view", view);
        phongShader.setMat4("transform", transform);
        phongShader.use();
    } 
    else {
        gouraudShader.setMat4("view", view);
        gouraudShader.setMat4("transform", transform);
        gouraudShader.use();
    }

    if (shadingMode == ShadingMode::FLAT) {
        glBindVertexArray(vaoFlat);
        glDrawArrays(GL_TRIANGLES, 0, nVerticesFlat);
    }
    else {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, (GLvoid*)0);
    }
}

int main()
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sandbox", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    createGeometry();

    std::string vertexShader = util::readFile("../shaders/phong.vert");
    std::string fragmentShader = util::readFile("../shaders/phong.frag");
    Shader phongShader(vertexShader, fragmentShader);

    vertexShader = util::readFile("../shaders/gouraud.vert");
    fragmentShader = util::readFile("../shaders/gouraud.frag");
    Shader gouraudShader(vertexShader, fragmentShader);

    glm::mat4 projection = camera.projectionMatrix();
    gouraudShader.setMat4("projection", projection);
    gouraudShader.setVec3("inLightDir", sunDir);
    gouraudShader.setFloat("inAmbientLight", ambientLight);

    phongShader.setMat4("projection", projection);
    phongShader.setVec3("inLightDir", sunDir);
    phongShader.setFloat("inAmbientLight", ambientLight);

    lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render(gouraudShader, phongShader);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    deleteGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}