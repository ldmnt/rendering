#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 front, right, up;

    float nearPlane, farPlane;
    float fov;
    float aspectRatio;

public:
    Camera(glm::vec3 front, float nearPlane, float farPlane, float aspectRatio, float fov, glm::vec3 position = glm::vec3(0.0f));

    glm::vec3 getPosition();
    void moveForward(float);
    void moveRight(float);
    void turn(float counterClockwiseAngle, float risingAngle);
    void lookAt(glm::vec3 target);
    glm::mat4 viewMatrix();
    glm::mat4 projectionMatrix();

private:
    void updateVectors();
};

class SphericalCamera {
private:
    glm::mat4 rotation;

    float nearPlane, farPlane;
    float fov;
    float aspectRatio;

public:
    enum class MovementType { FORWARD, RIGHT, UP, HORIZONTAL_ROTATION, VERTICAL_ROTATION }; // rotations are counter-clockwise is view space
    SphericalCamera(float nearPlane, float farPlane, float aspectRatio, float fov, glm::mat4 rotation = glm::mat4(1.0f));

    glm::vec4 getPosition();
    void move(float angle, MovementType type);
    glm::mat4 viewMatrix();
    glm::mat4 projectionMatrix();
};

