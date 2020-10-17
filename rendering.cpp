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

const float CAMERA_SPEED = 2.5f;

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

glm::vec3 cameraPos(4, 3, 2);
glm::vec3 cameraAim = glm::normalize(glm::vec3(0.0f) - cameraPos);
glm::vec3 cameraRight = glm::normalize(glm::cross(cameraAim, glm::vec3(0, 0, 1)));
glm::vec3 cameraUp = glm::cross(cameraRight, cameraAim);

Camera camera(cameraAim, cameraPos);

glm::vec3 sunDir = glm::vec3(-100, 0, 0);

float nearPlane = 0.02f;
float farPlane = 1000.0f;
float fieldOfView = 45.0f;
glm::mat4 projection = glm::perspective(glm::radians(fieldOfView), 640.0f / 480.0f, nearPlane, farPlane);

GLuint vao, vbo, ebo, shaderProgram;
float lastFrame, deltaTime;

std::string readFile(const char* filePath) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(filePath);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        return stream.str();
    }
    catch (std::ifstream::failure& e) {
        fprintf(stderr, "IO error: %s\n", e.what());
        return "";
    }
}

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void deleteGeometry() {
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

static int compileShader(GLenum type, const char* source) {
    GLuint shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &source, NULL);
    glCompileShader(shaderId);
    GLint success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint maxLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);
        char *log = (new std::vector<GLchar>(maxLength))->data();
        glGetShaderInfoLog(shaderId, maxLength, &maxLength, log);
        fprintf(stderr, "Error: %s\n", log);
        glDeleteShader(shaderId);
        return 0;
    }
    return shaderId;
}

static int linkShaders(std::string vertexSource, std::string fragmentSource) {
    int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource.c_str());
    int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);
    GLint success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        GLint maxLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);
        char* log = (new std::vector<GLchar>(maxLength))->data();
        glGetProgramInfoLog(programId, maxLength, &maxLength, log);
        fprintf(stderr, "Error: %s\n", log);
        glDeleteProgram(programId);
        programId = 0;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return programId;
}

static void render() {
    
    glm::mat4 view = camera.viewMatrix();
    
    int viewLoc = glGetUniformLocation(shaderProgram, "view");

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
}

static void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera.moveForward(CAMERA_SPEED * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera.moveForward(- CAMERA_SPEED * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.moveRight(CAMERA_SPEED * deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.moveRight(- CAMERA_SPEED * deltaTime);
    }
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
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    createGeometry();
    std::string vertexShader = readFile("../rendering/shaders/shader.vert");
    std::string fragmentShader = readFile("../rendering/shaders/shader.frag");
    shaderProgram = linkShaders(vertexShader, fragmentShader);

    glUseProgram(shaderProgram);
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    deleteGeometry();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}