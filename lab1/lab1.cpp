#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderLoader.h"
#include "Model.h"
#include "Mesh.h"

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;

GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.03f;
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);
}

void setMat4(int shaderID, const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Ошибка - не запустить GLFW" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D модель", nullptr, nullptr);
    if (!window) {
        std::cerr << "Ошибка - не запустить окно GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Ошибка - не запустить GLEW" << std::endl;
        return 1;
    }

    // Загрузка шейдеров
    GLuint shaderProgram = ShaderLoader::LoadShader("vertex_shader.glsl", "fragment_shader.glsl");
    if (!shaderProgram) {
        std::cerr << "Ошибка при загрузке шейдеров" << std::endl;
        return 1;
    }

    glm::mat4 translateOX1 = glm::mat4(1.0f);
    float posOX1 = 0.0f; // Позиция по Y
    glm::mat4 rotateOX2 = glm::mat4(1.0f);
    glm::mat4 translateOX3 = glm::mat4(1.0f);
    float posOX3 = 0.0f;


    float angleOX1 = 0.0f;
    float angleOX2 = 0.0f;
    float angleOX3 = 0.0f;

    Model ourModel("models/lab3BOB.obj");

    glm::vec3 cameraPos(0.0f, 0.0f, 4.0f);
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
    glm::vec3 pivotPoint = glm::vec3(-0.5f, 0.5f, 0.0f);

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(SCR_WIDTH) / SCR_HEIGHT,
        0.1f,
        100.0f
    );

    // Основной цикл
    while (!glfwWindowShouldClose(window)) {
        const float cameraSpeed = 0.002f;
        const float rotationSpeed = 0.005f;
        const float moveSpeed = 0.0005f;
        const float MIN_Y = -2.0f;
        const float MAX_Y = 0.3f;
        const float MIN_Z_OX3 = -0.6f;
        const float MAX_Z_OX3 = 0.2f;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && posOX1 > MIN_Y) {
            posOX1 -= moveSpeed;
            translateOX1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, posOX1));
        }

        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && posOX1 < MAX_Y) {
            posOX1 += moveSpeed;
            translateOX1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, posOX1));
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && angleOX2 > 0) {
            angleOX2 -= rotationSpeed;
            rotateOX2 =
                glm::translate(glm::mat4(1.0f), pivotPoint) *
                glm::rotate(glm::mat4(1.0f), glm::radians(angleOX2), glm::vec3(0.0, 0.0, 1.0)) *
                glm::translate(glm::mat4(1.0f), -pivotPoint);
        }

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && angleOX2 < 10) {
            angleOX2 += rotationSpeed;
            rotateOX2 =
                glm::translate(glm::mat4(1.0f), pivotPoint) *
                glm::rotate(glm::mat4(1.0f), glm::radians(angleOX2), glm::vec3(0.0, 0.0, 1.0)) *
                glm::translate(glm::mat4(1.0f), -pivotPoint);
        }

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && posOX3 > MIN_Z_OX3) {
            posOX3 -= moveSpeed;
            translateOX3 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, posOX3, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && posOX3 < MAX_Z_OX3) {
            posOX3 += moveSpeed;
            translateOX3 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, posOX3, 0.0f));
        }

        glm::mat4 baseTransform = glm::mat4(1.0f);
        glm::mat4 xo1Transform = translateOX1;
        glm::mat4 xo2Transform = rotateOX2;
        glm::mat4 xo3Transform = translateOX3;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Активация шейдера и установка uniform-переменных
        glUseProgram(shaderProgram);
        setMat4(shaderProgram, "projection", projection);
        setMat4(shaderProgram, "view", view);

        // Настройка освещения
        glUniform3f(glGetUniformLocation(shaderProgram, "Light_1.ambient"), 0.3f, 0.3f, 0.3f);
        glUniform3f(glGetUniformLocation(shaderProgram, "Light_1.diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shaderProgram, "Light_1.specular"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "Light_1.position"), cameraPos.x, cameraPos.y + 1.0f, cameraPos.z + 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

        // Настройка материала
        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.24725f, 0.1995f, 0.0745f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.75164f, 0.60648f, 0.22648f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.628281f, 0.555802f, 0.366065f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 51.2f);

        // Отрисовка модели
        glm::mat4 model = glm::mat4(1.0f);
        setMat4(shaderProgram, "model", model);
        ourModel.Draw(shaderProgram, glm::mat4(1.0f), translateOX1, rotateOX2, translateOX3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}