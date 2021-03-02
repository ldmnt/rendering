#include <GL/glew.h>
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
#include <assimp/Importer.hpp>

#include "camera.h"
#include "util.h"
#include "shader.h"
#include "model.h"
#include "light.h"
#include "drawing.h"

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

static void render(Model &model, Shader &gouraudShader, Shader &phongShader) {
    Shader &shader = shadingMode == ShadingMode::PHONG ? phongShader : gouraudShader;

    glm::mat4 view = camera.viewMatrix();
    glm::mat4 transform = glm::mat4(1.0f);

    shader.setVec3("eyePos", camera.getPosition());
    shader.setMat4("view", view);
    shader.setMat4("transform", transform);
    shader.use();

    model.draw(shader, shadingMode == ShadingMode::FLAT);
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

    Model model("resources/sphere.obj", true);

    DirectionalLight dirLight(
        glm::vec3(-1, 0, -0.5f), 
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.8f, 0.8f, 0.8f)
    );

    std::string vertexShader = util::readFile("shaders/phong.vert");
    std::string fragmentShader = util::readFile("shaders/phong.frag");
    Shader phongShader(vertexShader, fragmentShader);

    vertexShader = util::readFile("shaders/gouraud.vert");
    fragmentShader = util::readFile("shaders/gouraud.frag");
    Shader gouraudShader(vertexShader, fragmentShader);

    glm::mat4 projection = camera.projectionMatrix();
    gouraudShader.setMat4("projection", projection);
    draw::setDirectionalLight(gouraudShader, "dirLight", dirLight);

    phongShader.setMat4("projection", projection);
    draw::setDirectionalLight(phongShader, "dirLight", dirLight);

    lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render(model, gouraudShader, phongShader);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
