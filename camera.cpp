#include <math.h>
#include "camera.h"

const glm::vec3 WORLD_UP(0.0f, 0.0f, 0.1f);

Camera::Camera(glm::vec3 front, glm::vec3 position) :
    position(position), front(glm::normalize(front)) {
    glm::vec3 projXY = glm::normalize(glm::vec3(front.x, front.y, 0.0f));
    yaw = glm::degrees(atan2(projXY.y, projXY.x));
    pitch = glm::degrees(asin(this->front.z));
    updateVectors();
}

void Camera::moveForward(float amount) {
    position += amount * front;
}

void Camera::moveRight(float amount) {
    position += amount * right;
}

void Camera::turn(float counterClockwiseAngle, float risingAngle) {
    pitch += risingAngle;
    if (pitch > 89.0f) 
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    yaw += counterClockwiseAngle;
    updateVectors();
}

void Camera::updateVectors() {
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front.z = sin(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, WORLD_UP));
    up = glm::cross(right, front);
}

glm::mat4 Camera::viewMatrix() {
    return glm::lookAt(position, position + front, up);
}