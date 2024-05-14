//#pragma once
//#include <GLM.hpp>
//#include <gtc/matrix_transform.hpp>
//
//#include "Shader.h"
//#include "Texture.h"
//#include "Vertex.h"
//#include "Loaders.h"
//
//#include <string>
//#include <vector>
//
//using namespace std;
//
//class Mesh
//{
//private:
//	std::vector <Vertex> vertices;
//	std::vector <GLuint> indices;
//	std::vector <Material> materials;
//
//	GLuint VAO;
//	GLuint VBO;
//	GLuint EBO;
//	glm::mat4 ModelMatrix;
//	glm::vec3 position;
//	glm::vec3 rotation;
//	glm::vec3 scale;
//
//	void initVertexData(Vertex* vertexArray, const unsigned& nrOfVertices, GLuint* indexArray, const unsigned& nrOfIndices);
//	void updateModelMatrix();
//	void initMaterials();
//
//public:
//	Mesh(std::string OBJfile, std::string PathFile);
//	~Mesh();
//	void initVAO();
//	void render(Shader* shader);
//	void setPosition(glm::vec3 position);
//	void setRotation(glm::vec3 rotation);
//	void setModel(glm::mat4 Model);
//	void setScale(glm::vec3 scale);
//	void setColor(int index, glm::vec3 rgb);
//	glm::mat4 getModel();
//	glm::vec3 getRotation();
//	glm::vec3 getPosition();
//	std::vector <Material> getMaterials();
//};
//
#pragma once

#include <vector>
#include <GL/glew.h>
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <glfw3.h>// Include glad to get the required OpenGL headers
#include <string>
#include "Shader.h" 

struct Vertex
{
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
    // Tangent
    glm::vec3 Tangent;
    // Bitangent
    glm::vec3 Bitangent;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    // Mesh Data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader& shader); // Render the mesh

private:
    // Render data
    unsigned int VAO, VBO, EBO;

    // Initializes all the buffer objects/arrays
    void setupMesh();
};