#ifndef MESH_H
#define MESH_H

#include "ShaderLoader.h"
#include <GL\GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    unsigned int VAO;
    enum GroupID { BASE, XO1, XO2, XO3 };
    GroupID groupID;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, GroupID group)
        : vertices(vertices), indices(indices), groupID(group)
    {
        setupMesh();
    }

    void Draw(GLuint shaderProgram,
        const glm::mat4& baseTransform,
        const glm::mat4& xo1Transform,
        const glm::mat4& xo2Transform,
        const glm::mat4& xo3Transform)
    {
        glm::mat4 finalTransform = baseTransform;

        switch (groupID) {
        case XO1:
            finalTransform = finalTransform * xo1Transform;
            break;
        case XO2:
            finalTransform = finalTransform * xo1Transform * xo2Transform;
            break;
        case XO3:
            finalTransform = finalTransform * xo1Transform * xo2Transform * xo3Transform;
            break;
        case BASE:
        default:
            break;
        }

        ShaderLoader::SetMat4(shaderProgram, "model", finalTransform);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int VBO, EBO;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Установка атрибутов вершин
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glBindVertexArray(0);
    }
};
#endif