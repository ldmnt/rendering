#include <glm/matrix.hpp>
#include <math.h>
#include "camera.h"

const glm::vec3 WORLD_UP(0.0f, 0.0f, 0.1f);

Camera::Camera(glm::vec3 front, float nearPlane, float farPlane, float aspectRatio, float fov, glm::vec3 position) :
    position(position), front(glm::normalize(front)), nearPlane(nearPlane), farPlane(farPlane), aspectRatio(aspectRatio), fov(fov) {
    updateVectors();
}

glm::vec3 Camera::getPosition() {
    return position;
}

void Camera::moveForward(float amount) {
    position += amount * front;
}

void Camera::moveRight(float amount) {
    position += amount * right;
}

void Camera::turn(float counterClockwiseAngle, float risingAngle) {
    glm::vec3 projXY = glm::normalize(glm::vec3(front.x, front.y, 0.0f));
    float yaw = glm::degrees(atan2(projXY.y, projXY.x));
    float pitch = glm::degrees(asin(this->front.z));
    pitch += risingAngle;
    if (pitch > 89.0f) 
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    yaw += counterClockwiseAngle;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front.z = sin(glm::radians(pitch));
    front = glm::normalize(front);

    updateVectors();
}

void Camera::updateVectors() {
    right = glm::normalize(glm::cross(front, WORLD_UP));
    up = glm::cross(right, front);
}

void Camera::lookAt(glm::vec3 target) {
    front = glm::normalize(target - position);
    updateVectors();
}

glm::mat4 Camera::viewMatrix() {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::projectionMatrix() {
    return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}


SphericalCamera::SphericalCamera(float nearPlane, float farPlane, float aspectRatio, float fov, glm::mat4 rotation) :
    rotation(rotation), nearPlane(nearPlane), farPlane(farPlane), aspectRatio(aspectRatio), fov(fov) {}

glm::vec4 SphericalCamera::getPosition() {
    return rotation[3];
}

void SphericalCamera::move(float angle, MovementType type) {
    glm::mat4 displacement;
    switch (type) {
    case MovementType::FORWARD:
        displacement = glm::mat4(
            glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, cos(angle), sin(angle)),
            glm::vec4(0.0f, 0.0f, -sin(angle), cos(angle))
        );
        break;
    case MovementType::RIGHT:
        displacement = glm::mat4(
            glm::vec4(cos(-angle), 0.0f, 0.0f, sin(-angle)),
            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
            glm::vec4(-sin(-angle), 0.0f, 0.0f, cos(-angle))
        );
        break;
    case MovementType::UP:
        displacement = glm::mat4(
            glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, cos(angle), 0.0f, sin(angle)),
            glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
            glm::vec4(0.0f, -sin(angle), 0.0f, cos(angle))
        );
        break;
    }

    rotation = rotation * displacement;
    
//    glm::vec4 right = rotation[0];
//    glm::vec4 up = rotation[1];
//    glm::vec4 back = rotation[2];
//    glm::vec4 position = rotation[3];
//
}

glm::mat4 SphericalCamera::viewMatrix() {
    return glm::transpose(rotation);
}

glm::mat4 SphericalCamera::projectionMatrix() {
    return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}
