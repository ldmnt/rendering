#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 worldUp;

public:
    Camera(glm::vec3 front, glm::vec3 position = glm::vec3(0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f));

    void moveForward(float);
    void moveRight(float);
    glm::mat4 viewMatrix();

private:
    void updateVectors();
};

