#include "camera.h"

Camera::Camera(glm::vec3 front, glm::vec3 position, glm::vec3 worldUp) :
    position(position), front(glm::normalize(front)), worldUp(glm::normalize(worldUp)) {
    updateVectors();
}

void Camera::moveForward(float amount) {
    position += amount * front;
}

void Camera::moveRight(float amount) {
    position += amount * right;
}

void Camera::updateVectors() {
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::cross(right, front);
}

glm::mat4 Camera::viewMatrix() {
    return glm::lookAt(position, position + front, up);
}