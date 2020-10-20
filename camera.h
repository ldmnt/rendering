#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 front, right, up;
    float pitch, yaw;

public:
    Camera(glm::vec3 front, glm::vec3 position = glm::vec3(0.0f));

    void moveForward(float);
    void moveRight(float);
    void turn(float counterClockwiseAngle, float risingAngle);
    void rotateAroundOrigin(float counterClockwiseAngle, float risingAngle);
    glm::mat4 viewMatrix();

private:
    void updateVectors();
    void updateVectorsFromFront();
};

