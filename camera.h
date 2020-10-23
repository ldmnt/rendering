#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 front, right, up;
    float pitch, yaw;

    float nearPlane, farPlane;
    float fov;
    float aspectRatio;

public:
    Camera(glm::vec3 front, float nearPlane, float farPlane, float aspectRatio, float fov, glm::vec3 position = glm::vec3(0.0f));

    void moveForward(float);
    void moveRight(float);
    void turn(float counterClockwiseAngle, float risingAngle);
    void rotateAroundOrigin(float counterClockwiseAngle, float risingAngle);
    void lookAt(glm::vec3 target);
    glm::mat4 viewMatrix();
    glm::mat4 projectionMatrix();

private:
    void updateVectors();
    void updateMembersFromFront();
};

