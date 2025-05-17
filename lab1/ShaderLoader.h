#ifndef SHADER_LOADER_H  
#define SHADER_LOADER_H  

#include <GL/glew.h>  
#include <string>  
#include <fstream>  
#include <sstream>  
#include <iostream>  

class ShaderLoader {
public:
    // ������� ��� �������� ��������  
    static GLuint LoadShader(const std::string& vertexPath, const std::string& fragmentPath) {
        // ������ ����� ��������  
        std::string vertexCode = ReadFile(vertexPath);
        std::string fragmentCode = ReadFile(fragmentPath);
        // �������� �������� ��������
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        // �������� ��������� ���� ��������
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // ���������� ��������
        glShaderSource(vertexShader, 1, &vShaderCode, NULL);
        glCompileShader(vertexShader);
        CheckCompileErrors(vertexShader, "VERTEX");

        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);
        CheckCompileErrors(fragmentShader, "FRAGMENT");

        // �������� ��������� ���������  
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        CheckProgramErrors(shaderProgram);

        // �������� ��������, ��� ������ �� ����� ����� ������
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    // ������� ��� ��������� uniform ���������� float  
    static void SetUniform1f(GLuint program, const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(program, name.c_str()), value);
    }

    // ������� ��� ��������� uniform ������� 4x4
    static void SetMat4(GLuint program, const std::string& name, const glm::mat4& mat) {
        glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // ������� ��� ������ �����  
    static std::string ReadFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "������ ������� ����. ������ " << filePath << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    // �������� ������ ���������� ��������  
    static void CheckCompileErrors(GLuint shader, const std::string& type) {
        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "������ ���������� ��������.\n" << infoLog << "\n" << std::endl;
        }
    }

    // �������� ������ ������ ���������
    static void CheckProgramErrors(GLuint program) {
        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 1024, NULL, infoLog);
            std::cerr << "������ ������ ���������.\n" << infoLog << "\n" << std::endl;
        }
    }
};

#endif