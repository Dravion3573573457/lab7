#ifndef MODEL_H
#define MODEL_H

#include <GL\GL.h>
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "ShaderLoader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

class Model {
public:
    vector<Mesh> meshes;
    string directory;

    Model(string const& path) {
        loadModel(path);
        directory = path.substr(0, path.find_last_of('/'));
    }

    void Draw(GLuint shaderProgram,
        const glm::mat4& baseTransform,
        const glm::mat4& group1Transform,
        const glm::mat4& group2Transform,
        const glm::mat4& group3Transform)
    {
        for (auto& mesh : meshes) {
            mesh.Draw(shaderProgram, baseTransform,
                group1Transform, group2Transform, group3Transform);
        }
    }

private:
    void loadModel(string const& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene) {
        Mesh::GroupID groupID = Mesh::BASE;
        if (strstr(node->mName.C_Str(), "XO1")) groupID = Mesh::XO1;
        else if (strstr(node->mName.C_Str(), "XO2")) groupID = Mesh::XO2;
        else if (strstr(node->mName.C_Str(), "XO3")) groupID = Mesh::XO3;

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene, groupID));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene, Mesh::GroupID groupID) {
        vector<Vertex> vertices;
        vector<unsigned int> indices;

        if (!mesh->mVertices) {
            cerr << "WARNING: Mesh has no vertices!" << endl;
            return Mesh(vertices, indices, groupID);
        }

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            if (mesh->mNormals) {
                vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            }
            else {
                vertex.Normal = glm::vec3(0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return Mesh(vertices, indices, groupID);
    }
};

#endif