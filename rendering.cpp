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

const float CAMERA_SPEED = 2.5f;
const float MOUSE_SENSITIVITY = 0.3f;

const float PROJECTION_NEAR_PLANE = 0.02f;
const float PROJECTION_FAR_PLANE = 1000.0f;
const float PROJECTION_FOV = 45.0f;

static float triangle[] = {
//  position   color     normal
    0, 0, 0,  1, 0, 0,  0, -1, 0,
    0, 0, 0,  0, 1, 0,  -1, 0, 0,
    0, 0, 0,  0, 0, 1,  0, 0, -1,
    1, 0, 0,  1, 1, 0,  1, 0, 0,
    1, 0, 0,  1, 0, 0,  0, -1, 0,
    1, 0, 0,  0, 0, 1,  0, 0, -1,
    0, 1, 0,  1, 0, 1,  0, 1, 0,
    0, 1, 0,  0, 1, 0,  -1, 0, 0,
    0, 1, 0,  0, 0, 1,  0, 0, -1,
    0, 0, 1,  0, 1, 1,  0, 0, 1,
    0, 0, 1,  1, 0, 0,  0, -1, 0,
    0, 0, 1,  0, 1, 0,  -1, 0, 0,
    1, 1, 0,  1, 0, 1,  0, 1, 0,
    1, 1, 0,  0, 0, 1,  0, 0, -1,
    1, 1, 0,  1, 1, 0,  1, 0, 0,
    0, 1, 1,  1, 0, 1,  0, 1, 0,
    0, 1, 1,  0, 1, 0,  -1, 0, 0,
    0, 1, 1,  0, 1, 1,  0, 0, 1,
    1, 0, 1,  1, 0, 0,  0, -1, 0,
    1, 0, 1,  1, 1, 0,  1, 0, 0,
    1, 0, 1,  0, 1, 1,  0, 0, 1,
    1, 1, 1,  1, 0, 1,  0, 1, 0,
    1, 1, 1,  1, 1, 0,  1, 0, 0,
    1, 1, 1,  0, 1, 1,  0, 0, 1,
};

static GLuint indices[] = { 
    2, 8, 5, 
    5, 8, 13, 
    18, 10, 0, 
    18, 0, 4,
    16, 7, 1,
    11, 16, 1,
    17, 9, 20,
    23, 17, 20,
    15, 21, 12,
    6, 15, 12,
    22, 19, 3,
    14, 22, 3,
};

bool firstMouse = true;
double lastMouseX, lastMouseY;

bool fixedCamera = true;
glm::vec3 cameraPos(4, 3, 2);
glm::vec3 cameraAim = glm::normalize(glm::vec3(0.0f) - cameraPos);
Camera camera(cameraAim, cameraPos);

glm::vec3 sunDir = glm::vec3(-1, -1, -1);
float ambientLight = 0.2f;

GLuint vao, vbo, ebo;
float lastFrame, deltaTime;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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
        xOffset /= 100.0f;
        yOffset /= 100.0f;
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
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void deleteGeometry() {
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

static void render(Shader shader) {
    glm::mat4 view = camera.viewMatrix();
    shader.setMat4("view", view);

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));
    shader.setMat4("transform", transform);
    
    glBindVertexArray(vao);
    shader.use();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
}

int main()
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Sandbox", NULL, NULL);
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

    createGeometry();

    std::string vertexShader = util::readFile("../shaders/flat.vert");
    std::string fragmentShader = util::readFile("../shaders/flat.frag");
    Shader shader(vertexShader, fragmentShader);

    glm::mat4 projection = glm::perspective(
        glm::radians(PROJECTION_FOV), 
        640.0f / 480.0f, 
        PROJECTION_NEAR_PLANE, 
        PROJECTION_FAR_PLANE
    );
    shader.setMat4("projection", projection);
    shader.setVec3("lightDir", sunDir);
    shader.setFloat("ambientLight", ambientLight);

    lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render(shader);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    deleteGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}