#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:
    GLuint id;

public:
    Shader(std::string vertexSource, std::string fragmentSource);
    ~Shader();
    void use();
    void setMat4(const char* name, glm::mat4 value);
    void setVec3(const char* name, glm::vec3 value);
    void setFloat(const char* name, float value);

private:
    int compileShader(GLenum type, const char* source);
    int linkShaders(std::string vertexSource, std::string fragmentSource);
};
