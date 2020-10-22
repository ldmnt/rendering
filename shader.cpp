#include "shader.h"
#include <vector>


Shader::Shader() : id(0) {}

Shader::Shader(std::string vertexSource, std::string fragmentSource) {
    id = linkShaders(vertexSource, fragmentSource);
}

Shader::~Shader() {
    glDeleteProgram(id);
}

void Shader::use() {
    glUseProgram(id);
}

void Shader::setMat4(const char* name, glm::mat4 value) {
    use();
    int loc = glGetUniformLocation(id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const char* name, glm::vec3 value) {
    use();
    int loc = glGetUniformLocation(id, name);
    glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Shader::setFloat(const char* name, float value) {
    use();
    int loc = glGetUniformLocation(id, name);
    glUniform1f(loc, value);
}

int Shader::compileShader(GLenum type, const char* source) {
    GLuint shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &source, NULL);
    glCompileShader(shaderId);
    GLint success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint maxLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);
        char* log = (new std::vector<char>(maxLength))->data();
        glGetShaderInfoLog(shaderId, maxLength, &maxLength, log);
        char* typeStr = type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
        fprintf(stderr, "Shader Compile time error (%s): %s\n", typeStr, log);
        glDeleteShader(shaderId);
        return 0;
    }
    return shaderId;
}

int Shader::linkShaders(std::string vertexSource, std::string fragmentSource) {
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
        char* log = (new std::vector<char>(maxLength))->data();
        glGetProgramInfoLog(programId, maxLength, &maxLength, log);
        fprintf(stderr, "Shader link time error: %s\n", log);
        glDeleteProgram(programId);
        programId = 0;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return programId;
}